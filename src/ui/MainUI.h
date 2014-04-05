// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef MainUI_h
#define MainUI_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "Workspace.h"
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>

class MainUI {
public:
  Fl_Double_Window* make_window();
  Fl_Double_Window *window;
  Workspace *workspace;
  static Fl_Menu_Item menu_[];
  static Fl_Menu_Item *mnu_file_open;
  static Fl_Menu_Item *mnu_file_save;
  static Fl_Menu_Item *mnu_file_saveas;
  static Fl_Menu_Item *mnu_file_exit;
  static Fl_Menu_Item *mnu_edit_select_all;
  static Fl_Menu_Item *mnu_edit_select_none;
  static Fl_Menu_Item *mnu_edit_cut;
  static Fl_Menu_Item *mnu_edit_copy;
  static Fl_Menu_Item *mnu_edit_paste;
  static Fl_Menu_Item *mnu_edit_dup;
  static Fl_Menu_Item *mnu_edit_group;
  static Fl_Menu_Item *mnu_edit_ungroup;
  Fl_Scrollbar *scroll_h;
  Fl_Scrollbar *scroll_v;
  Fl_Button *tb_up;
  Fl_Button *tb_down;
  Fl_Tree *node_tree;
  Fl_Scroll *properties;
  Fl_Box *group_name;
};
#endif
