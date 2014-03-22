#include <FL/Fl.H>

#include "core/Graph.h"
#include "pkgs/logic/BitInput.h"
#include "pkgs/logic/BitOutput.h"
#include "pkgs/logic/AndGate.h"

#include "MainUI.h"

static MainUI ui;

static void setup()
{
	Graph * graph = new Graph("Simple");

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

	// XXX: for testing, save to xml file
	graph->save_to("tmp.xml");

	ui.workspace->graph(graph);
}

int main(int argc, const char * argv[])
{
	Fl::lock();
	Fl::scheme("gtk+");
	ui.make_window()->show();
	setup();
	Fl::run();
	return 0;
}
