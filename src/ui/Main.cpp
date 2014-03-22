#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>

#include "core/Graph.h"
#include "pkgs/logic/BitInput.h"
#include "pkgs/logic/BitOutput.h"
#include "pkgs/logic/AndGate.h"

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
