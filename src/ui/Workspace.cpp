/*
 * Workspace.cpp
 *
 *  Created on: Mar 22, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>

#include "core/Util.h"
#include "core/NodeFactory.h"

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
	: Fl_Widget(x, y, w, h, l), _graph(new Graph("Untitled")), _high(NULL),
	  _start_x(0), _start_y(0), _sel_count(0),
	  _sel_conn_node(NULL), _sel_conn_idx(-1),
	  _state(Idle), _scroll_x(0), _scroll_y(0),
	  _scr_h(NULL), _scr_v(NULL)
{
}

Workspace::~Workspace()
{
	clear();
}

void Workspace::clear()
{
	_nodes.clear();
	_conns.clear();
	delete _graph;
	_graph = NULL;
}

void Workspace::scrollbars(Fl_Scrollbar * h, Fl_Scrollbar * v)
{
	_scr_h = h;
	_scr_v = v;
	set_scrollbar_range();
	_scr_h->callback(cb_scroll_x, this);
	_scr_v->callback(cb_scroll_y, this);
}

void Workspace::cb_scroll_x(Fl_Widget * w, void * d)
{
	Workspace * thiz = (Workspace*) d;
	thiz->_scroll_x = thiz->_scr_h->value();
	thiz->redraw();
}

void Workspace::cb_scroll_y(Fl_Widget * w, void * d)
{
	Workspace * thiz = (Workspace*) d;
	thiz->_scroll_y = thiz->_scr_v->value();
	thiz->redraw();
}

void Workspace::set_scrollbar_range()
{
	if (!_graph || !_scr_h || !_scr_v) return;
	int minx, miny, maxx, maxy;
	_graph->calc_range(minx, miny, maxx, maxy);
	_scr_h->value(_scroll_x, w(), minx, maxx - minx);
	_scr_v->value(_scroll_y, h(), miny, maxy - miny);
}


void Workspace::graph(Graph * graph)
{
	clear();
	_graph = graph;

	for (int i = 0; i < _graph->node_count(); i++) {
		NodeUI * node = new NodeUI(_graph->node(i));
		_nodes.add(node);
	}

	for (int i = 0; i < _graph->connection_count(); i++) {
		_conns.add(new ConnectionUI(_graph->connection(i)));
	}

	set_scrollbar_range();
	redraw();
}

void Workspace::draw()
{
	fl_push_clip(x(), y(), w(), h());
	draw_background();
	draw_nodes();
	draw_connections();
	fl_pop_clip();
}

int Workspace::handle(int event)
{
	int ret = 0, dx = 0, dy = 0, in_idx, out_idx;
	int mx = s2gx(Fl::event_x());
	int my = s2gy(Fl::event_y());
	NodeUI * node = NULL;
	switch (event) {
	case FL_ENTER:
		ret = 1;
		break;
	case FL_LEAVE:
		ret = 1;
		fl_cursor(FL_CURSOR_DEFAULT);
		break;
	case FL_MOVE:
		node = find_node_below(mx, my);
		in_idx = node ? node->find_input(mx, my) : -1;
		out_idx = node ? node->find_output(mx, my) : -1;
		highlight(node);
		if (in_idx >= 0 || out_idx >= 0) {
			fl_cursor(FL_CURSOR_HAND);
		} else if (node) {
			fl_cursor(FL_CURSOR_MOVE);
		} else {
			fl_cursor(FL_CURSOR_DEFAULT);
		}
		ret = 1;
		break;
	case FL_PUSH:
		_start_x = mx;
		_start_y = my;
		node = find_node_below(_start_x, _start_y);
		in_idx = node ? node->find_input(_start_x, _start_y) : -1;
		out_idx = node ? node->find_output(_start_x, _start_y) : -1;
		if (in_idx >= 0) {
			ConnectionUI * conn = find_connection_to(node, in_idx);
			if (conn) {
				start_connection_drag(conn->conn()->from(), conn->conn()->out_idx(), DragIn);
				delete_connection(conn);
			} else {
				start_connection_drag(node->node(), in_idx, DragOut);
			}
		} else if (out_idx >= 0) {
			ConnectionUI * conn = find_connection_from(node, out_idx);
			if (conn) {
				start_connection_drag(conn->conn()->to(), conn->conn()->in_idx(), DragOut);
				delete_connection(conn);
			} else {
				start_connection_drag(node->node(), out_idx, DragIn);
			}
		} else if (node) {
			select_node(node, Fl::event_shift());
			if (_sel_count) {
				_state = WaitForDrag;
			}
		} else {
			_state = WaitForPan;
		}
		return 1;
	case FL_DRAG:
		dx = mx - _start_x;
		dy = my - _start_y;
		if (_state == WaitForDrag && check_drag_dist(dx, dy)) {
			_state = Drag;
		}
		if (_state == Drag) {
			drag_selected(dx, dy);
			_start_x += dx;
			_start_y += dy;
		} else if (_state == DragIn || _state == DragOut) {
			_start_x += dx;
			_start_y += dy;
			redraw();
		}
		return 1;
	case FL_RELEASE:
		if (_state == WaitForPan) {
			// Clicked on the workspace, deselect everything
			unselect_all();
		} else if (_state == DragIn) {
			node = find_node_below(_start_x, _start_y);
			int in_idx = node ? node->find_input(_start_x, _start_y) : -1;
			if (node && in_idx >= 0) {
				add_connection(_sel_conn_node, _sel_conn_idx, node, in_idx);
			}
		} else if (_state == DragOut) {
			node = find_node_below(_start_x, _start_y);
			int out_idx = node ? node->find_output(_start_x, _start_y) : -1;
			if (node && out_idx >= 0) {
				add_connection(node, out_idx, _sel_conn_node, _sel_conn_idx);
			}
		} else if (_state == Drag) {
			set_scrollbar_range();
			_graph->dirty(1);
		}
		_state = Idle;
		redraw();
		return 1;
	}
	if (Fl_Widget::handle(event)) ret = 1;
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
	// FIXME: scroll
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
	x0 = g2sx(x0);
	y0 = g2sy(y0);
	x1 = g2sx(x1);
	y1 = g2sy(y1);
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

void Workspace::draw_nodes()
{
	for (int i = 0; i < _nodes.count(); i++) {
		_nodes[i]->draw(g2sx(0), g2sy(0));
	}
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

	if (_state == DragIn) {
		int x0 = _sel_conn_node->node()->output_x(_sel_conn_idx);
		int y0 = _sel_conn_node->node()->output_y(_sel_conn_idx);
		draw_connection(x0, y0, _start_x, _start_y, 72, 88);
	}

	if (_state == DragOut) {
		int x0 = _sel_conn_node->node()->input_x(_sel_conn_idx);
		int y0 = _sel_conn_node->node()->input_y(_sel_conn_idx);
		draw_connection(_start_x, _start_y, x0, y0, 72, 88);
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

void Workspace::unselect_all()
{
	for (int i = 0; i < _nodes.count(); i++) {
		_nodes[i]->selected(0);
	}
	_sel_count = 0;
	redraw();
}

ConnectionUI * Workspace::find_connection_to(const NodeUI * node, int in_idx)
{
	const Node * n = node->node();
	for (int i = 0; i < _conns.count(); i++) {
		Connection * conn = _conns[i]->conn();
		if (conn->to() == n && conn->in_idx() == in_idx) {
			return _conns[i];
		}
	}
	return NULL;
}

ConnectionUI * Workspace::find_connection_from(const NodeUI * node, int out_idx)
{
	const Node * n = node->node();
	for (int i = 0; i < _conns.count(); i++) {
		Connection * conn = _conns[i]->conn();
		if (conn->from() == n && conn->out_idx() == out_idx) {
			return _conns[i];
		}
	}
	return NULL;
}

void Workspace::start_connection_drag(const Node * node, int idx, State state)
{
	_sel_conn_node = find_node(node);
	_sel_conn_idx = idx;
	_state = state;
	redraw();
}

void Workspace::delete_connection(ConnectionUI * conn)
{
	_graph->remove(conn->conn());
	_conns.remove(conn);
	redraw();
}

void Workspace::add_connection(const NodeUI * from, int out_idx, const NodeUI * to, int in_idx)
{
	// Need to remove old connection to the input
	ConnectionUI * old = find_connection_to(to, in_idx);
	if (old) {
		delete_connection(old);
	}

	// Add new connection
	Connection * conn = new Connection(from->node(), out_idx, to->node(), in_idx);
	_graph->add(conn);
	ConnectionUI * connui = new ConnectionUI(conn);
	_conns.add(connui);
	redraw();
}

const NodeUI * Workspace::find_node(const Node * node)
{
	for (int i = 0; i < _nodes.count(); i++) {
		if (_nodes[i]->node() == node) {
			return _nodes[i];
		}
	}
	return NULL;
}

void Workspace::add_node(const char * name)
{
	int x = _scroll_x + w() / 2;
	int y = _scroll_y + h() / 2;
	Node * node = new_node(name, x, y);
	if (!node) {
		fl_alert("Internal error: cannot create node!");
		return;
	}
	_graph->add(node);
	NodeUI * nodeui = new NodeUI(node);
	_nodes.add(nodeui);
	redraw();
}
