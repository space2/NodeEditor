#include <FL/Fl.H>

#include "MainUI.h"

static MainUI ui;

int main(int argc, const char * argv[])
{
	Fl::lock();
	Fl::scheme("gtk+");
	ui.make_window()->show();
	Fl::run();
	return 0;
}
