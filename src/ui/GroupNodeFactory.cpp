/*
 * GroupNodeFactory.cpp
 *
 *  Created on: Apr 5, 2014
 *      Author: Pal Szasz <pal.szasz@gmail.com>
 */

#include "ui/GroupNodeFactory.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>
#include <dirent.h>

GroupNodeFactory::GroupNodeFactory(Group * grp)
	: NodeFactory(kMacroGroup, grp->name())
{
	pugi::xml_node n = _xml.append_child("node");
	n.append_attribute("type").set_value(grp->type());
	n.append_attribute("x").set_value(grp->x());
	n.append_attribute("y").set_value(grp->y());
	grp->save_to(n);
}

GroupNodeFactory::GroupNodeFactory(const char * fn)
	: NodeFactory(kMacroGroup, NULL)
{
	if (_xml.load_file(fn).status == pugi::status_ok) {
		this->type(_xml.first_child().attribute("name").as_string());
	}
}

GroupNodeFactory::~GroupNodeFactory()
{
}

Node * GroupNodeFactory::create_new(int x, int y)
{
	pugi::xml_node child = _xml.first_child();
	const char * name = child.attribute("type").as_string();
	Node * clone = new_node(name, x, y);
	clone->load_from(child);
	return clone;
}

int GroupNodeFactory::export_to_file()
{
	char fn[1024];
	struct passwd *pw = getpwuid(getuid());
	strcpy(fn, pw->pw_dir);
	int err;

	// Make sure folder is created
	strcat(fn, "/.NodeEditor");
	err = mkdir(fn, 0700);
	if (err && errno != EEXIST) {
		perror("mkdir");
		fprintf(stderr, "Error creating private folder (%s)\n", fn);
		return 0;
	}

	// Make sure sub-folder is created
	strcat(fn, "/Macros");
	err = mkdir(fn, 0700);
	if (err && errno != EEXIST) {
		perror("mkdir");
		fprintf(stderr, "Error creating private sub-folder (%s)\n", fn);
		return 0;
	}

	// Save the file itself
	strcat(fn, "/");
	strcat(fn, type());
	strcat(fn, ".xml");
	if (!_xml.save_file(fn)) {
		fprintf(stderr, "Error creating xml file (%s)\n", fn);
		return 0;
	}

	return 1;
}

void GroupNodeFactory::scan_exported_files()
{
	char fn[1024];
	struct passwd *pw = getpwuid(getuid());
	strcpy(fn, pw->pw_dir);
	strcat(fn, "/.NodeEditor/Macros");
	DIR * dir = opendir(fn);
	if (dir) {
		while (1) {
			struct dirent * entry = readdir(dir);
			if (!entry) break;
			if (entry->d_name[0] == '.') continue;

			// Check if it's an xml file
			const char * xml_fn = entry->d_name;
			int l = strlen(xml_fn);
			if (strcmp(xml_fn + l - 4, ".xml")) continue;

			// Open xml file
			char full_xml_fn[1024];
			sprintf(full_xml_fn, "%s/%s", fn, xml_fn);
			GroupNodeFactory * nf = new GroupNodeFactory(full_xml_fn);
			if (nf->type()) {
				register_node_factory(nf);
			} else {
				delete nf;
			}
		}
		closedir(dir);
	}
}
