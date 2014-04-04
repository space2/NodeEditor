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
#include "core/GroupNode.h"
#include "core/GroupPorts.h"

#include "Workspace.h"
#include "DnD.h"

static const int kGridSize = 10;
static const int kGridMajor = 10;
static const int kGridMajorSize = 5;

static const int kDragTreshold = 10;

static const int kSlotColorsDefault[] = {40, 46, 50, FL_DARK3};
static const int kSlotColors0[] = {40, 7, 7, FL_DARK3};
static const int kSlotColors1[] = {40, 0, 0, FL_DARK3};

static const int * choose_slot_colors(const Slot * slot)
{
	if (slot->type() == Slot::Bit) {
		return slot->as_bit() ? kSlotColors1 : kSlotColors0;
	}
	return kSlotColorsDefault;
}

static int check_drag_dist(int dx, int dy)
{
	if (Util::abs(dx) > kDragTreshold) return 1;
	if (Util::abs(dy) > kDragTreshold) return 1;
	return 0;
}

static int check_dnd_add()
{
	const char * clip = Fl::event_text();
	if (0 == strncmp(clip, kDndPrefixAdd, strlen(kDndPrefixAdd))) {
		return 1;
	}
	return 0;
}

Workspace::Workspace(int x, int y, int w, int h, const char * l)
	: Fl_Widget(x, y, w, h, l), _graph(new Graph("Untitled")), _high(NULL),
	  _start_x(0), _start_y(0), _end_x(0), _end_y(0), _sel_count(0),
	  _sel_conn_node(NULL), _sel_conn_idx(-1),
	  _state(Idle), _scroll_x(0), _scroll_y(0),
	  _scr_h(NULL), _scr_v(NULL),
	  _cb(NULL)
{
}

Workspace::~Workspace()
{
	clear();
}

void Workspace::clear()
{
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
	set_scrollbar_range();
	_sel_count = 0;
	if (_cb) _cb(NodeSelected, NULL);
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
	Node * node = NULL;
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
			Connection * conn = find_connection_to(node, in_idx);
			if (conn) {
				start_connection_drag(conn->from(), conn->out_idx(), DragIn);
				delete_connection(conn);
			} else {
				start_connection_drag(node, in_idx, DragOut);
			}
		} else if (out_idx >= 0) {
			Connection * conn = find_connection_from(node, out_idx);
			if (conn) {
				start_connection_drag(conn->to(), conn->in_idx(), DragOut);
				delete_connection(conn);
			} else {
				start_connection_drag(node, out_idx, DragIn);
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
	case FL_DND_ENTER:
	case FL_DND_DRAG:
	case FL_DND_RELEASE:
		return 1;
	case FL_PASTE:
		if (check_dnd_add()) {
			add_node(Fl::event_text() + strlen(kDndPrefixAdd), mx, my);
		}
		break;
	}
	if (Fl_Widget::handle(event)) ret = 1;
	return ret;
}

void Workspace::drag_selected(int dx, int dy)
{
	for (int i = 0; i < _graph->node_count(); i++) {
		Node * node = _graph->node(i);
		if (node->selected()) {
			node->move(dx, dy);
		}
	}
	redraw();
}

Node * Workspace::find_node_below(int x, int y)
{
	for (int i = _graph->node_count()-1; i >= 0; i--) {
		Node * node = _graph->node(i);
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
	for (int i = 0; i < _graph->node_count(); i++) {
		draw_node(_graph->node(i), g2sx(0), g2sy(0));
	}
}

int Workspace::node_color(Node * node)
{
	if (node->selected()) {
		return _high == node ? 167 : 166;
	} else {
		return _high == node ? 51 : 49;
	}
}

void Workspace::draw_node(Node * node, int dx, int dy)
{
	int x = node->x() + dx, y = node->y() + dy, w = node->w(), h = node->h();

	// Render box
	fl_draw_box(FL_UP_BOX, x, y, w, h, node_color(node));

	// Render label
	fl_color(FL_BLACK);
	fl_font(FL_HELVETICA, 10);
	fl_draw(node->name(), x, y, w, h, FL_ALIGN_TOP | FL_ALIGN_INSIDE, NULL, 0);

	// Draw extra overlays
	fl_color(FL_DARK2);
	fl_xyline(x + 2, y + 13, x + w - 2);
	fl_font(FL_HELVETICA, 8);
	for (int i = 0; i < node->input_count(); i++) {
		const int * slot_colors = choose_slot_colors(node->input(i));
		int xx = x;
		int yy = dy + node->input_y(i);
		fl_color(slot_colors[0]);
		fl_pie(xx-4, yy-4, 9, 9, -90, 90);
		fl_color(slot_colors[1]);
		fl_pie(xx-3, yy-3, 7, 7, -90, 90);
		fl_color(slot_colors[2]);
		fl_line(xx+1, yy-3, xx+1, yy+3);
		fl_color(slot_colors[3]);
		fl_draw(node->input(i)->name(), xx + 5, yy + 1);
	}
	for (int i = 0; i < node->output_count(); i++) {
		const int * slot_colors = choose_slot_colors(node->output(i));
		int xx = x + w - 1;
		int yy = dy + node->output_y(i);
		fl_color(slot_colors[0]);
		fl_pie(xx-4, yy-4, 9, 9, 90, 270);
		fl_color(slot_colors[1]);
		fl_pie(xx-3, yy-3, 7, 7, 90, 270);
		fl_color(slot_colors[2]);
		fl_line(xx, yy-3, xx, yy+3);
		fl_color(slot_colors[3]);
		int tw = 0, th = 0;
		fl_measure(node->output(i)->name(), tw, th, 0);
		fl_draw(node->output(i)->name(), xx - 5 - tw, yy + 1);
	}

	// Draw value
	const Slot * slot = node->edit_slot();
	if (!slot) slot = node->show_slot();
	if (slot) {
		if (slot->type() == Slot::Bit) {
			const char * v = Util::bit2string(slot->as_bit());
			fl_color(FL_BLACK);
			fl_font(FL_COURIER, 16);
			fl_draw(v, x, y + 15, w, h - 15, FL_ALIGN_CENTER, NULL, 0);
		} else {
			// TODO: Render the other types
		}
	}
}

void Workspace::draw_connections()
{
	for (int i = 0; i < _graph->connection_count(); i++) {
		Connection * conn = _graph->connection(i);
		int x0 = conn->from()->output_x(conn->out_idx());
		int y0 = conn->from()->output_y(conn->out_idx());
		int x1 = conn->to()->input_x(conn->in_idx());
		int y1 = conn->to()->input_y(conn->in_idx());
		draw_connection(x0, y0, x1, y1, 58, 79);
	}

	if (_state == DragIn) {
		int x0 = _sel_conn_node->output_x(_sel_conn_idx);
		int y0 = _sel_conn_node->output_y(_sel_conn_idx);
		draw_connection(x0, y0, _start_x, _start_y, 72, 88);
	}

	if (_state == DragOut) {
		int x0 = _sel_conn_node->input_x(_sel_conn_idx);
		int y0 = _sel_conn_node->input_y(_sel_conn_idx);
		draw_connection(_start_x, _start_y, x0, y0, 72, 88);
	}
}

void Workspace::highlight(Node * node)
{
	_high = node;
	redraw();
}

void Workspace::select_node(Node * node, int toggle)
{
	if (!node) return;
	if (toggle) {
		node->selected(!node->selected());
		_sel_count += node->selected() ? 1 : -1;
		if (_cb) _cb(NodeSelected, NULL);
		redraw();
	} else {
		if (node->selected()) return; // NOP
		for (int i = 0; i < _graph->node_count(); i++) {
			_graph->node(i)->selected(0);
		}
		node->selected(1);
		_sel_count = 1;
		if (_cb) _cb(NodeSelected, node);
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
	for (int i = 0; i < _graph->node_count(); i++) {
		Node * n = _graph->node(i);
		if (n->x() > x1) continue;
		if (n->y() > y1) continue;
		if (n->x() + n->w() < x0) continue;
		if (n->y() + n->h() < y0) continue;
		if (op == SelectAdd || op == SelectSet) {
			if (!n->selected()) {
				n->selected(1);
				_sel_count++;
			}
		} else {
			if (n->selected()) {
				n->selected(0);
				_sel_count--;
			}
		}
	}
	if (_cb) _cb(NodeSelected, NULL);
	redraw();
}

void Workspace::select_all()
{
	for (int i = 0; i < _graph->node_count(); i++) {
		_graph->node(i)->selected(1);
	}
	_sel_count = _graph->node_count();
	if (_cb) _cb(NodeSelected, NULL);
	redraw();
}

void Workspace::unselect_all()
{
	for (int i = 0; i < _graph->node_count(); i++) {
		_graph->node(i)->selected(0);
	}
	_sel_count = 0;
	if (_cb) _cb(NodeSelected, NULL);
	redraw();
}

Connection * Workspace::find_connection_to(const Node * node, int in_idx)
{
	for (int i = 0; i < _graph->connection_count(); i++) {
		Connection * conn = _graph->connection(i);
		if (conn->to() == node && conn->in_idx() == in_idx) {
			return _graph->connection(i);
		}
	}
	return NULL;
}

Connection * Workspace::find_connection_from(const Node * node, int out_idx)
{
	for (int i = 0; i < _graph->connection_count(); i++) {
		Connection * conn = _graph->connection(i);
		if (conn->from() == node && conn->out_idx() == out_idx) {
			return _graph->connection(i);
		}
	}
	return NULL;
}

void Workspace::start_connection_drag(Node * node, int idx, State state)
{
	_sel_conn_node = node;
	_sel_conn_idx = idx;
	_state = state;
	redraw();
}

void Workspace::delete_connection(Connection * conn)
{
	_graph->remove(conn);
	redraw();
}

void Workspace::add_connection(Node * from, int out_idx, Node * to, int in_idx)
{
	// Need to remove old connection to the input
	Connection * old = find_connection_to(to, in_idx);
	if (old) {
		delete_connection(old);
	}

	// Add new connection
	Connection * conn = new Connection(from, out_idx, to, in_idx);
	_graph->add(conn);
	redraw();
}

void Workspace::add_node(const char * name, int x, int y)
{
	Node * node = new_node(name, x, y);
	if (!node) {
		fl_alert("Internal error: cannot create node!");
		return;
	}
	_graph->add(node);
	_graph->calc();
	redraw();
}

void Workspace::cut()
{
	copy();
	if (!_sel_count) return;
	for (int i = _graph->node_count()-1; i >= 0; i--) {
		if (!_graph->node(i)->selected()) continue;
		remove(_graph->node(i));
	}
	_sel_count = 0;
	_graph->calc();
	if (_cb) _cb(NodeSelected, NULL);
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
	int cnt = _graph->node_count();
	for (int i = 0; i < cnt; i++) {
		if (!_graph->node(i)->selected()) continue;
		Node * old = _graph->node(i);
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
		clone->selected(1);
		_sel_count++;
		child = child.next_sibling();
	}
	_graph->calc();
	if (_cb) _cb(NodeSelected, NULL);
	redraw();
}

void Workspace::duplicate()
{
	if (!_sel_count) {
		fl_alert("No nodes selected!");
		return;
	}
	int cnt = _graph->node_count();
	for (int i = 0; i < cnt; i++) {
		if (!_graph->node(i)->selected()) continue;
		Node * old = _graph->node(i);
		Node * clone = new_node(old->name(), old->x() + 10, old->y() + 10);
		if (!clone) continue; // Maybe we should log this?
		pugi::xml_node tmp;
		old->save_to(tmp);
		clone->load_from(tmp);
		_graph->add(clone);
		_graph->node(i)->selected(0);
		clone->selected(1);
	}
	_graph->calc();
	redraw();
}

void Workspace::remove(Node * node)
{
	_graph->remove(node);
}

void Workspace::group()
{
	if (!_sel_count) {
		fl_alert("No nodes selected!");
		return;
	}

	// Calculate bounding box of selected nodes
	int min_x = 0, min_y = 0, max_x = 0, max_y = 0;
	for (int i = 0; i < _graph->node_count(); i++) {
		Node * node = _graph->node(i);
		if (!node->selected()) continue;
		min_x = Util::min(min_x, node->x());
		min_y = Util::min(min_y, node->y());
		max_x = Util::max(max_x, node->x() + node->w());
		max_y = Util::max(max_y, node->y() + node->h());
	}

	// Create group node and add to graph
	Group * grp = new Group("Group");
	GroupNode * grp_node = new GroupNode((min_x + max_x) / 2, (min_y + max_y) / 2, grp);
	_graph->add(grp_node);
	GroupPorts * grp_inputs = new GroupPorts(min_x, (min_y + max_y) / 2);
	grp_node->inports(grp_inputs);
	GroupPorts * grp_outputs = new GroupPorts(max_x, (min_y + max_y) / 2);
	grp_node->outports(grp_outputs);

	// Copy selected nodes to new group node
	Array<Node*> nodes_to_delete;
	int cnt = _graph->node_count();
	for (int i = 0; i < cnt; i++) {
		if (!_graph->node(i)->selected()) continue;
		Node * node = _graph->node(i);
		grp->add(node);
		nodes_to_delete.add(node);
	}

	// Copy connections which are completely in the selection
	Array<Connection*> conns_to_delete;
	for (int i = 0; i < _graph->connection_count(); i++) {
		Connection * conn = _graph->connection(i);
		if (conn->from()->selected() && conn->to()->selected()) {
			grp->add(conn);
			conns_to_delete.add(conn);
		}
	}

	// For every connections which is between a selected and not-selected node,
	// add a port to the group, and connect it
	for (int i = 0; i < _graph->connection_count(); i++) {
		Connection * conn = _graph->connection(i);
		if (conn->from()->selected() && conn->to()->selected()) continue; // Already processed
		if (conn->from()->selected()) {
			Slot * old_output = conn->from()->output(conn->out_idx());
			Slot * old_input = conn->to()->input(conn->in_idx());
			int new_in_idx = grp_outputs->copy_input(old_input);
			int new_out_idx = grp_node->copy_output(old_output);
			Connection * int_conn = new Connection(conn->from(), conn->out_idx(), grp_outputs, new_in_idx);
			grp->add(int_conn);
			conn->from(grp_node, new_out_idx);
		} else if (conn->to()->selected()) {
			Slot * old_output = conn->from()->output(conn->out_idx());
			Slot * old_input = conn->to()->input(conn->in_idx());
			int new_in_idx = grp_inputs->copy_output(old_output);
			int new_out_idx = grp_node->copy_input(old_input);
			Connection * int_conn = new Connection(grp_inputs, new_out_idx, conn->to(), conn->in_idx());
			grp->add(int_conn);
			conn->to(grp_node, new_in_idx);
		}
	}

	// Cleanup
	for (int i = 0; i < nodes_to_delete.count(); i++) {
		_graph->remove(nodes_to_delete[i], 1);
	}
	for (int i = 0; i < conns_to_delete.count(); i++) {
		_graph->remove(conns_to_delete[i], 1);
	}

	// Recalculate network
	_graph->calc();
	redraw();
}

void Workspace::ungroup()
{
	// TODO
}
