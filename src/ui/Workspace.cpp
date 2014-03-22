/*
 * Workspace.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include "core/Util.h"

#include "Workspace.h"
#include "NodeUI.h"

static const int kGridSize = 10;
static const int kGridMajor = 10;
static const int kGridMajorSize = 5;

static const int kDragTreshold = 10;

static int check_drag_dist(int dx, int dy)
{
	if (Util::abs(dx) > kDragTreshold) return 1;
	if (Util::abs(dy) > kDragTreshold) return 1;
	return 0;
}

Workspace::Workspace(int x, int y, int w, int h, const char * l)
	: Fl_Group(x, y, w, h, l), _graph(new Graph("Untitled")), _high(NULL),
	  _start_x(0), _start_y(0), _sel_count(0), _state(Idle)
{
	end();
}

Workspace::~Workspace()
{
	clear();
}

void Workspace::clear()
{
	for (int i = children()-1; i >= 0; i--) {
		remove(i);
	}
	_nodes.clear();
	_conns.clear();
	delete _graph;
	_graph = NULL;
}

void Workspace::graph(Graph * graph)
{
	clear();
	_graph = graph;

	for (int i = 0; i < _graph->node_count(); i++) {
		NodeUI * node = new NodeUI(_graph->node(i));
		_nodes.add(node);
		add(node);
	}

	for (int i = 0; i < _graph->connection_count(); i++) {
		_conns.add(new ConnectionUI(_graph->connection(i)));
	}

	redraw();
}

void Workspace::draw()
{
	draw_background();
	draw_children();
	draw_connections();
}

int Workspace::handle(int event)
{
	int ret = 0;
	NodeUI * node = NULL;
	switch (event) {
	case FL_ENTER:
		ret = 1;
		break;
	case FL_MOVE:
		highlight(find_node_below(Fl::event_x(), Fl::event_y()));
		ret = 1;
		break;
	case FL_PUSH:
		_start_x = Fl::event_x();
		_start_y = Fl::event_y();
		node = find_node_below(_start_x, _start_y);
		if (node) {
			select_node(node, Fl::event_shift());
			if (_sel_count) {
				_state = WaitForDrag;
			}
		} else {
			_state = WaitForPan;
		}
		return 1;
	case FL_DRAG:
		int dx = Fl::event_x() - _start_x;
		int dy = Fl::event_y() - _start_y;
		if (_state == WaitForDrag && check_drag_dist(dx, dy)) {
			_state = Drag;
		}
		if (_state == Drag) {
			drag_selected(dx, dy);
			_start_x += dx;
			_start_y += dy;
		}
		return 1;
	}
	if (Fl_Group::handle(event)) ret = 1;
	return ret;
}

void Workspace::drag_selected(int dx, int dy)
{
	for (int i = 0; i < _nodes.count(); i++) {
		NodeUI * node = _nodes[i];
		if (node->selected()) {
			node->move(dx, dy);
		}
	}
	redraw();
}

NodeUI * Workspace::find_node_below(int x, int y)
{
	for (int i = 0; i < _nodes.count(); i++) {
		NodeUI * node = _nodes[i];
		if (node->inside(x, y)) {
			return node;
		}
	}
	return NULL;
}

void Workspace::draw_background()
{
	fl_rectf(x(), y(), w(), h(), FL_BLACK);
	fl_color(36);
	for (int xx = x(); xx < x() + w(); xx += kGridSize) {
		fl_line(xx, y(), xx, y() + h());
	}
	for (int yy = y(); yy < y() + h(); yy += kGridSize) {
		fl_line(x(), yy, x() + w(), yy);
	}
	fl_color(40);
	for (int xx = x(); xx < x() + w(); xx += kGridSize * kGridMajor) {
		for (int yy = y(); yy < y() + h(); yy += kGridSize * kGridMajor) {
			fl_line(xx - kGridMajorSize, yy, xx + kGridMajorSize, yy);
			fl_line(xx, yy - kGridMajorSize, xx, yy + kGridMajorSize);
		}
	}
}

void Workspace::draw_connection(int x0, int y0, int x1, int y1, int col0, int col1)
{
	fl_line_style(FL_SOLID | FL_CAP_ROUND | FL_JOIN_ROUND, 3);
	fl_color(col0);
	fl_begin_line();
	fl_curve(x0, y0, x0 + 100, y0, x1 - 100, y1, x1, y1);
	fl_end_line();
	fl_line_style(FL_SOLID | FL_CAP_ROUND | FL_JOIN_ROUND, 1);
	fl_color(col1);
	fl_begin_line();
	fl_curve(x0, y0, x0 + 100, y0, x1 - 100, y1, x1, y1);
	fl_end_line();
}

void Workspace::draw_connections()
{
	for (int i = 0; i < _conns.count(); i++) {
		Connection * conn = _conns[i]->conn();
		int x0 = conn->from()->output_x(conn->out_idx());
		int y0 = conn->from()->output_y(conn->out_idx());
		int x1 = conn->to()->input_x(conn->in_idx());
		int y1 = conn->to()->input_y(conn->in_idx());
		draw_connection(x0, y0, x1, y1, 58, 79);
	}
}

void Workspace::highlight(NodeUI * node)
{
	if (_high) {
		_high->highlighted(0);
	}
	_high = node;
	if (_high) {
		_high->highlighted(1);
	}
	redraw();
}

void Workspace::select_node(NodeUI * node, int toggle)
{
	if (!node) return;
	if (toggle) {
		node->selected(!node->selected());
		_sel_count += node->selected() ? 1 : -1;
		redraw();
	} else {
		if (node->selected()) return; // NOP
		for (int i = 0; i < _nodes.count(); i++) {
			_nodes[i]->selected(0);
		}
		node->selected(1);
		_sel_count = 1;
		redraw();
	}
}
