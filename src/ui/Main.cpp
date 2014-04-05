#include <stdio.h>

#include <assert.h>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "core/Graph.h"
#include "core/NodeFactory.h"

#include "MainUI.h"
#include "NodeLabel.h"

static const char * kWindowTitle = "NodeEditor";
static const int kPropertyLabelWidth = 100;
static const int kPropertyValueWidth = 100;
static const int kPropertyHeight = 25;

static MainUI ui;
static Fl_Native_File_Chooser file_chooser;
static Node * selected_node = NULL;
static Group * _selected_group = NULL;

static const char * kDummyDndText = "(DUMMY)";

class DraggableSlotName;

static DraggableSlotName * _dnd_slot_start = NULL;

class DraggableSlotName : public Fl_Group
{
public:
	DraggableSlotName(int x, int y, int w, int h, Node * node, int output, int idx)
		: Fl_Group(x, y, w, h, 0), _node(node), _output(output), _idx(idx)
	{}
protected:
	int handle(int event) {
		switch (event) {
		case FL_PUSH:
			if (Fl_Group::handle(event) == 0 && _node->can_reorder_slots()) {
				// No child handled it
				_dnd_slot_start = this;
				Fl::copy(kDummyDndText, strlen(kDummyDndText), 0);
				Fl::dnd();
				return 1;
			}
			break;
		case FL_DND_ENTER:
		case FL_DND_DRAG:
		case FL_DND_RELEASE:
			return 1;
			break;
		case FL_PASTE:
			if (0 == strcmp(kDummyDndText, Fl::event_text()) && _dnd_slot_start) {
				if (_node->can_reorder_slots()) {
					if (_dnd_slot_start->_node == _node && _dnd_slot_start->_output == _output) {
						ui.workspace->swap_slots(_node, _output, _dnd_slot_start->_idx, _idx);
					}
				}
				return 1;
			}
			_dnd_slot_start = NULL;
			break;
		}
		return Fl_Group::handle(event);
	}
private:
	Node * _node;
	int _output, _idx;
};

static void cb_port_name_changed(Fl_Widget * w, void * data)
{
	Slot * slot = (Slot *)data;
	Fl_Input * input = (Fl_Input *) w;
	slot->name(input->value());
	selected_node->dirty(1);
	ui.workspace->redraw();
}

static void cb_gate_name_changed(Fl_Widget * w, void * data)
{
	Fl_Input * input = (Fl_Input *) w;
	const char * name = input->value();
	if (name && !name[0]) name = NULL; // Optimize away empty string as null
	selected_node->name(name);
	ui.workspace->redraw();
}

static void add_gate_name_property(int x, int y, int output, int idx, Slot * slot)
{
	char label[32];
	sprintf(label, "%s#%d", output ? "Output" : "Input", idx);
	DraggableSlotName * grp = new DraggableSlotName(x, y, kPropertyLabelWidth + kPropertyValueWidth, kPropertyHeight, selected_node, output, idx);
	Fl_Input * value_w = new Fl_Input(x + kPropertyLabelWidth, y, kPropertyValueWidth, kPropertyHeight);
	grp->end();
	value_w->callback(cb_port_name_changed, slot);
	value_w->when(FL_WHEN_ENTER_KEY_CHANGED);
	value_w->copy_label(label);
	value_w->value(slot->name());
}

static void show_properties(Node * node)
{
	selected_node = node;
	ui.properties->clear();
	if (!node) return;
	ui.properties->begin();
	int x = ui.properties->x() + 2; // Gap due to box
	int y = ui.properties->y() + 2; // Gap due to box

	// Add node name
	Fl_Input * value_w = new Fl_Input(x + kPropertyLabelWidth, y, kPropertyValueWidth, kPropertyHeight, "Name");
	value_w->callback(cb_gate_name_changed, node);
	value_w->when(FL_WHEN_ENTER_KEY_CHANGED);
	value_w->value(node->name());
	y += kPropertyHeight;

	// Add input names
	for (int i = 0; i < node->input_count(); i++) {
		add_gate_name_property(x, y, 0, i, node->input(i));
		y += kPropertyHeight;
	}

	// Add output names
	for (int i = 0; i < node->output_count(); i++) {
		add_gate_name_property(x, y, 1, i, node->output(i));
		y += kPropertyHeight;
	}

	ui.properties->end();
	ui.properties->redraw();
}

static void update_window_title()
{
	Graph * graph = ui.workspace->graph();
	const char * fn = graph->file_name();
	char buff[256];
	if (fn) {
		snprintf(buff, sizeof(buff)-1, "%s - %s", kWindowTitle, fn);
	} else {
		snprintf(buff, sizeof(buff)-1, "%s", kWindowTitle);
	}
	ui.window->label(strdup(buff));
}

static void cb_win_open(Fl_Widget * w, void * d)
{
	// Confirm first
	if (ui.workspace->graph()->is_dirty()) {
		int ret = fl_choice("Are you sure you want to open a new file?\nThere are unsaved changes which will be lost!", "Cancel", "Yes", NULL);
		if (!ret) return;
	}

	// Pick new file
	file_chooser.title("Open file:");
	file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
	file_chooser.filter("Node XML files\t*.xml\nAll files\t*");
	if (file_chooser.show() == 0) {
		const char * fn = file_chooser.filename();
		if (fn) {
			Graph * graph = new Graph();
			if (graph->load_from_file(fn)) {
				ui.workspace->graph(graph);
				ui.tb_up->deactivate();
				update_window_title();
			}
		}
	}
}

static void cb_win_save_as(Fl_Widget * w = NULL, void * d = NULL)
{
	// Pick new file
	file_chooser.title("Save file as:");
	file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	file_chooser.filter("Node XML files\t*.xml\nAll files\t*");
	if (file_chooser.show() == 0) {
		const char * fn = file_chooser.filename();
		if (fn) {
			ui.workspace->graph()->save_to_file(fn);
			update_window_title();
		}
	}
}

static void cb_win_save(Fl_Widget * w = NULL, void * d = NULL)
{
	Graph * graph = ui.workspace->graph();
	const char * fn = graph->file_name();
	if (fn) {
		graph->save_to_file(fn);
	} else {
		cb_win_save_as();
	}
}

static void cb_win_close(Fl_Widget * w, void * d)
{
	if (ui.workspace->graph()->is_dirty()) {
		int ret = fl_choice("Are you sure you want to exit?\nThere are unsaved changes which will be lost!", "Cancel", "Yes", NULL);
		if (!ret) return;
	}
	ui.window->hide();
}

static void cb_edit_select_all(Fl_Widget * w, void * d)
{
	ui.workspace->select_all();
}

static void cb_edit_select_none(Fl_Widget * w, void * d)
{
	ui.workspace->unselect_all();
}

static void cb_edit_cut(Fl_Widget * w, void * d)
{
	ui.workspace->cut();
}

static void cb_edit_copy(Fl_Widget * w, void * d)
{
	ui.workspace->copy();
}

static void cb_edit_paste(Fl_Widget * w, void * d)
{
	ui.workspace->paste();
}

static void cb_edit_dup(Fl_Widget * w, void * d)
{
	ui.workspace->duplicate();
}

static void cb_edit_group(Fl_Widget * w, void * d)
{
	ui.workspace->group_selected();
}

static void cb_edit_ungroup(Fl_Widget * w, void * d)
{
	ui.workspace->ungroup_selected();
}

static void cb_tb_up(Fl_Widget * w, void * d)
{
	Group * group = ui.workspace->group();
	Group * new_group = group->parent();
	if (new_group == NULL) {
		fl_alert("No parent!");
		return;
	}
	ui.workspace->group(new_group);
	if (new_group->parent()) {
		ui.tb_up->activate();
	} else {
		ui.tb_up->deactivate();
	}
}

static void cb_tb_down(Fl_Widget * w, void * d)
{
	if (_selected_group == NULL) {
		fl_alert("No group selected!");
		return;
	}
	ui.workspace->group(_selected_group);
	ui.tb_up->activate();
}

static void check_selected_group(Node * param)
{
	_selected_group = dynamic_cast<Group*>(param);
	if (_selected_group) {
		ui.tb_down->activate();
	} else {
		ui.tb_down->deactivate();
	}
}

static void cb_workspace(Workspace::CallbackEvent event, Node * param)
{
	if (event == Workspace::NodeSelected) {
		show_properties(param);
		check_selected_group(param);
		_dnd_slot_start = NULL;
	}
}

static void setup_graph()
{
	Graph * graph = new Graph();

#if 1
	graph->load_from_file("data/example_logic_simple.xml");
#else
	BitInput * inp1 = new BitInput(10, 10);
	BitInput * inp2 = new BitInput(10, 100);
	BitInput * inp3 = new BitInput(10, 190);
	AndGate * and_gate1 = new AndGate(200, 100);
	AndGate * and_gate2 = new AndGate(300, 200);
	BitOutput * out = new BitOutput(490, 200);

	graph->add(inp1);
	graph->add(inp2);
	graph->add(inp3);
	graph->add(and_gate1);
	graph->add(and_gate2);
	graph->add(out);

	graph->add(new Connection(inp1, 0, and_gate1, 0));
	graph->add(new Connection(inp2, 0, and_gate1, 1));
	graph->add(new Connection(and_gate1, 0, and_gate2, 0));
	graph->add(new Connection(inp3, 0, and_gate2, 1));
	graph->add(new Connection(and_gate2, 0, out, 0));
#endif

	ui.workspace->graph(graph);
	update_window_title();
}

static void setup_window()
{
	ui.mnu_file_open->callback(cb_win_open);
	ui.mnu_file_save->callback(cb_win_save);
	ui.mnu_file_saveas->callback(cb_win_save_as);
	ui.mnu_file_exit->callback(cb_win_close);
	ui.window->callback(cb_win_close);

	ui.mnu_edit_select_all->callback(cb_edit_select_all);
	ui.mnu_edit_select_none->callback(cb_edit_select_none);
	ui.mnu_edit_cut->callback(cb_edit_cut);
	ui.mnu_edit_copy->callback(cb_edit_copy);
	ui.mnu_edit_paste->callback(cb_edit_paste);
	ui.mnu_edit_dup->callback(cb_edit_dup);
	ui.mnu_edit_group->callback(cb_edit_group);
	ui.mnu_edit_ungroup->callback(cb_edit_ungroup);

	ui.tb_up->callback(cb_tb_up);
	ui.tb_up->deactivate();
	ui.tb_down->callback(cb_tb_down);
	ui.tb_down->deactivate();

	ui.workspace->scrollbars(ui.scroll_h, ui.scroll_v);
	ui.workspace->listener(cb_workspace);

	ui.node_tree->showroot(0);
	ui.node_tree->selectmode(FL_TREE_SELECT_SINGLE);
	ui.node_tree->marginleft(0);
	ui.node_tree->margintop(0);
	ui.node_tree->begin();
	for (int i = 0; i < node_count(); i++) {
		if (node_group(i)[0] == '_') continue; // These are hidden!
		char buff[256];
		snprintf(buff, sizeof(buff)-1, "%s/%s", node_group(i), node_name(i));
		Fl_Tree_Item * item = ui.node_tree->add(buff);
		NodeLabel * label = new NodeLabel(1, 1, 150, 1, node_name(i));
		item->widget(label);
	}
	ui.node_tree->end();
}

int main(int argc, const char * argv[])
{
	Fl::lock();
	Fl::scheme("gtk+");
	ui.make_window();
	setup_graph();
	setup_window();
	ui.window->show();
	Fl::run();
	return 0;
}
