/*
 * Workspace.h
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#pragma once

#include <FL/Fl_Widget.H>
#include <FL/Fl_Scrollbar.H>

#include "core/Graph.h"
#include "core/Array.h"

#include "NodeUI.h"
#include "ConnectionUI.h"

class Workspace : public Fl_Widget {
public:
	Workspace(int x, int y, int w, int h, const char * l = 0);
	virtual ~Workspace();

	void graph(Graph * graph);
	Graph * graph() { return _graph; }

	void scrollbars(Fl_Scrollbar * h, Fl_Scrollbar * v);

	void add_node(const char * name);

	void cut();
	void copy();
	void paste();
	void duplicate();
	void select_all();
	void unselect_all();
protected:
	void draw();
	int handle(int event);
private:
	enum State {
		Idle = 0,
		Edit,
		WaitForDrag,
		Drag,
		DragIn,
		DragOut,
		Select,
	};

	enum SelectOp {
		SelectSet,
		SelectAdd,
		SelectRemove,
	};

	void draw_background();
	void draw_connections();
	void draw_nodes();
	void draw_connection(int x0, int y0, int x1, int y1, int col0, int col1);
	void draw_selection();
	void clear();
	NodeUI * find_node_below(int x, int y);
	void highlight(NodeUI * node);
	void select_node(NodeUI * node, int toggle);
	void select_nodes_in_rect(int x0, int y0, int x1, int y1, SelectOp op);
	void drag_selected(int dx, int dy);
	ConnectionUI * find_connection_to(const NodeUI * node, int in_idx);
	ConnectionUI * find_connection_from(const NodeUI * node, int out_idx);
	void start_connection_drag(Node * node, int idx, State state);
	void delete_connection(ConnectionUI * conn);
	void add_connection(NodeUI * from, int out_idx, NodeUI * to, int in_idx);
	NodeUI * find_node(Node * node);
	void set_scrollbar_range();
	void remove(NodeUI * nodeui);

	int s2gx(int xx) { return xx - x() + _scroll_x; }
	int s2gy(int yy) { return yy - y() + _scroll_y; }
	int g2sx(int xx) { return xx - _scroll_x + x(); }
	int g2sy(int yy) { return yy - _scroll_y + y(); }

	static void cb_scroll_x(Fl_Widget * w, void * d);
	static void cb_scroll_y(Fl_Widget * w, void * d);

	Graph * _graph;
	ArrayO<NodeUI*> _nodes;
	ArrayO<ConnectionUI*> _conns;
	NodeUI * _high;
	int _start_x, _start_y, _end_x, _end_y, _sel_count;
	NodeUI * _sel_conn_node;
	int _sel_conn_idx;
	State _state;
	int _scroll_x, _scroll_y;
	Fl_Scrollbar * _scr_h;
	Fl_Scrollbar * _scr_v;
	pugi::xml_document _clipboard;
};

