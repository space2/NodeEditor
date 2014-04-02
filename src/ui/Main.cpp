#include <stdio.h>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "core/Graph.h"
#include "core/NodeFactory.h"

#include "MainUI.h"

static MainUI ui;
static Fl_Native_File_Chooser file_chooser;

static const char * kWindowTitle = "NodeEditor";

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

static void cb_add_node(Fl_Widget * w, void * d)
{
	const char * name = (const char *) d;
	ui.workspace->add_node(name);
}

static void cb_win_open(Fl_Widget * w, void * d)
{
	// Confirm first
	if (ui.workspace->graph()->dirty()) {
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
			Graph * graph = new Graph("Untitled");
			if (graph->load_from(fn)) {
				ui.workspace->graph(graph);
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
			ui.workspace->graph()->save_to(fn);
			update_window_title();
		}
	}
}

static void cb_win_save(Fl_Widget * w = NULL, void * d = NULL)
{
	Graph * graph = ui.workspace->graph();
	const char * fn = graph->file_name();
	if (fn) {
		graph->save_to(fn);
	} else {
		cb_win_save_as();
	}
}

static void cb_win_close(Fl_Widget * w, void * d)
{
	if (ui.workspace->graph()->dirty()) {
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

static void setup_graph()
{
	Graph * graph = new Graph("Simple");

#if 1
	graph->load_from("data/example_logic_simple.xml");
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

	ui.workspace->scrollbars(ui.scroll_h, ui.scroll_v);

	ui.node_tree->showroot(0);
	ui.node_tree->selectmode(FL_TREE_SELECT_SINGLE);
	ui.node_tree->marginleft(0);
	ui.node_tree->margintop(0);
	ui.node_tree->begin();
	for (int i = 0; i < node_count(); i++) {
		char buff[256];
		snprintf(buff, sizeof(buff)-1, "%s/%s", node_group(i), node_name(i));
		Fl_Tree_Item * item = ui.node_tree->add(buff);
		Fl_Button * btn = new Fl_Button(1, 1, 150, 1, node_name(i));
		btn->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
		item->widget(btn);
		btn->callback(cb_add_node, (void*)node_name(i));
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
