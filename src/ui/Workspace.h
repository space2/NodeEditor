/*
 * Workspace.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#ifndef WORKSPACE_H_
#define WORKSPACE_H_

#include <FL/Fl_Group.H>

#include "core/Graph.h"
#include "core/Array.h"

#include "NodeUI.h"
#include "ConnectionUI.h"

class Workspace : public Fl_Group {
public:
	Workspace(int x, int y, int w, int h, const char * l = 0);
	virtual ~Workspace();

	void graph(Graph * graph);
protected:
	void draw();
	int handle(int event);
private:
	void draw_background();
	void draw_connections();
	void draw_connection(int x0, int y0, int x1, int y1, int col0, int col1);
	void clear();
	NodeUI * find_node_below(int x, int y);
	void highlight(NodeUI * node);

	Graph * _graph;
	ArrayO<NodeUI*> _nodes;
	ArrayO<ConnectionUI*> _conns;
	NodeUI * _high;
};

#endif /* WORKSPACE_H_ */
