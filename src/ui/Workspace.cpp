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
	  _start_x(0), _start_y(0), _end_x(0), _end_y(0), _sel_count(0),
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
	_graph->calc();
	redraw();
}

void Workspace::draw()
{
	fl_push_clip(x(), y(), w(), h());
	draw_background();
	draw_nodes();
	draw_connections();
	draw_selection();
	fl_pop_clip();
}

void Workspace::draw_selection()
{
	if (_state != Select) return;
	int x = g2sx(Util::min(_start_x, _end_x));
	int y = g2sy(Util::min(_start_y, _end_y));
	int w = Util::abs(_end_x - _start_x);
	int h = Util::abs(_end_y - _start_y);
	fl_line_style(FL_SOLID, 1, NULL);
	fl_color(FL_BLACK);
	fl_rect(x, y, w, h);
	fl_line_style(FL_DASH, 1, NULL);
	fl_color(FL_RED);
	fl_rect(x, y, w, h);
	fl_line_style(FL_SOLID, 1, NULL);
	fl_color(FL_BLACK);
}

int Workspace::handle(int event)
{
	int ret = 0, dx = 0, dy = 0, in_idx, out_idx, edit_area;
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
		edit_area = node ? node->is_edit_area(mx, my) : 0;
		highlight(node);
		if (edit_area) {
			fl_cursor(FL_CURSOR_INSERT);
		} else if (in_idx >= 0 || out_idx >= 0) {
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
		edit_area = node ? node->is_edit_area(mx, my) : 0;
		if (edit_area) {
			_state = Edit;
			_sel_conn_node = node;
		} else if (in_idx >= 0) {
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
			_state = Select;
			_end_x = _start_x;
			_end_y = _start_y;
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
		} else if (_state == Select) {
			_end_x = mx;
			_end_y = my;
			redraw();
		}
		return 1;
	case FL_RELEASE:
		if (_state == Select) {
			SelectOp op = SelectSet;
			if (Fl::event_shift()) op = SelectAdd;
			if (Fl::event_alt()) op = SelectRemove;
			select_nodes_in_rect(_start_x, _start_y, _end_x, _end_y, op);
		} else if (_state == DragIn) {
			node = find_node_below(_start_x, _start_y);
			int in_idx = node ? node->find_input(_start_x, _start_y) : -1;
			if (node && in_idx >= 0) {
				add_connection(_sel_conn_node, _sel_conn_idx, node, in_idx);
			}
			_graph->calc();
		} else if (_state == DragOut) {
			node = find_node_below(_start_x, _start_y);
			int out_idx = node ? node->find_output(_start_x, _start_y) : -1;
			if (node && out_idx >= 0) {
				add_connection(node, out_idx, _sel_conn_node, _sel_conn_idx);
			}
			_graph->calc();
		} else if (_state == Drag) {
			set_scrollbar_range();
			_graph->dirty(1);
		} else if (_state == Edit) {
			if (_sel_conn_node->edit()) {
				_graph->calc();
				redraw();
			}
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
	for (int i = _nodes.count()-1; i >= 0; i--) {
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

	int x0 = _scroll_x, x1 = _scroll_x + w();
	int y0 = _scroll_y, y1 = _scroll_y + h();

	// Draw minor line
	x0 -= (x0 % kGridSize);
	y0 -= (y0 & kGridSize);
	for (int xx = x0; xx < x1; xx += kGridSize) {
		fl_line(g2sx(xx), y(), g2sx(xx), y() + h());
	}
	for (int yy = y0; yy < y1; yy += kGridSize) {
		fl_line(x(), g2sy(yy), x() + w(), g2sy(yy));
	}

	// Draw major line
	x0 -= (x0 % (kGridSize*kGridMajor));
	y0 -= (y0 % (kGridSize*kGridMajor));
	fl_color(40);
	for (int xx = x0; xx < x1; xx += kGridSize * kGridMajor) {
		for (int yy = y0; yy < y1; yy += kGridSize * kGridMajor) {
			int sx = g2sx(xx), sy = g2sy(yy);
			fl_line(sx - kGridMajorSize, sy, sx + kGridMajorSize, sy);
			fl_line(sx, sy - kGridMajorSize, sx, sy + kGridMajorSize);
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

void Workspace::select_nodes_in_rect(int x0, int y0, int x1, int y1, SelectOp op)
{
	if (x0 > x1) Util::swap(x0, x1);
	if (y0 > y1) Util::swap(y0, y1);
	if (op == SelectSet) {
		unselect_all();
	}
	for (int i = 0; i < _nodes.count(); i++) {
		NodeUI * node = _nodes[i];
		Node * n = node->node();
		if (n->x() > x1) continue;
		if (n->y() > y1) continue;
		if (n->x() + n->w() < x0) continue;
		if (n->y() + n->h() < y0) continue;
		if (op == SelectAdd || op == SelectSet) {
			if (!node->selected()) {
				node->selected(1);
				_sel_count++;
			}
		} else {
			if (node->selected()) {
				node->selected(0);
				_sel_count--;
			}
		}
	}
	redraw();
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

void Workspace::start_connection_drag(Node * node, int idx, State state)
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

void Workspace::add_connection(NodeUI * from, int out_idx, NodeUI * to, int in_idx)
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

NodeUI * Workspace::find_node(Node * node)
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
	_graph->calc();
	redraw();
}

void Workspace::cut()
{
	copy();
	if (!_sel_count) return;
	for (int i = _nodes.count()-1; i >= 0; i--) {
		if (!_nodes[i]->selected()) continue;
		remove(_nodes[i]);
	}
	_sel_count = 0;
	_graph->calc();
	redraw();
}

void Workspace::copy()
{
	if (!_sel_count) {
		fl_alert("No nodes selected!");
		return;
	}
	_clipboard.reset();
	pugi::xml_node root = _clipboard.append_child("clipboard");
	int cnt = _nodes.count();
	for (int i = 0; i < cnt; i++) {
		if (!_nodes[i]->selected()) continue;
		Node * old = _nodes[i]->node();
		pugi::xml_node n = root.append_child("node");
		n.append_attribute("name").set_value(old->name());
		n.append_attribute("x").set_value(old->x());
		n.append_attribute("y").set_value(old->y());
		old->save_to(n);
	}
}

void Workspace::paste()
{
	if (_clipboard.empty()) {
		fl_alert("Empty clipboard!");
		return;
	}
	unselect_all();
	pugi::xml_node root = _clipboard.first_child();
	pugi::xml_node child = root.first_child();
	while (!child.empty()) {
		const char * name = child.attribute("name").as_string();
		int x = child.attribute("x").as_int() + 10;
		int y = child.attribute("y").as_int() + 10;
		Node * clone = new_node(name, x, y);
		clone->load_from(child);
		_graph->add(clone);
		NodeUI * nodeui = new NodeUI(clone);
		_nodes.add(nodeui);
		nodeui->selected(1);
		_sel_count++;
		child = child.next_sibling();
	}
	_graph->calc();
	redraw();
}

void Workspace::duplicate()
{
	if (!_sel_count) {
		fl_alert("No nodes selected!");
		return;
	}
	int cnt = _nodes.count();
	for (int i = 0; i < cnt; i++) {
		if (!_nodes[i]->selected()) continue;
		Node * old = _nodes[i]->node();
		Node * clone = new_node(old->name(), old->x() + 10, old->y() + 10);
		if (!clone) continue; // Maybe we should log this?
		pugi::xml_node tmp;
		old->save_to(tmp);
		clone->load_from(tmp);
		_graph->add(clone);
		NodeUI * nodeui = new NodeUI(clone);
		_nodes.add(nodeui);

		_nodes[i]->selected(0);
		nodeui->selected(1);
	}
	_graph->calc();
	redraw();
}

void Workspace::remove(NodeUI * nodeui)
{
	Node * node = nodeui->node();
	for (int i = _conns.count()-1; i >= 0; i--) {
		Connection * conn = _conns[i]->conn();
		if (conn->from() == node || conn->to() == node) {
			_conns.remove_at(i);
		}
	}
	_graph->remove(node);
	_nodes.remove(nodeui);
}
