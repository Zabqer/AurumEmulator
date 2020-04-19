#ifndef __AURUM_FILESYSTEM_H__
#define __AURUM_FILESYSTEM_H__

#include <string>
#include <mutex>
#include <optional>
#include <set>
#include <fstream>

#include <yaml-cpp/yaml.h>

#include "component.h"

class FileSystem: public Component {
		public:
				class FileHandle: public Userdata {
						private:
								FileSystem* fs;
						public:
								std::string path;
								std::fstream stream;
								FileHandle(FileSystem*, std::string path, std::ios::openmode);
								~FileHandle();
				};
		private:
				std::string root;
				std::optional<std::string> label;;
				bool ro;
				int _spaceTotal;
				int _spaceUsed;
				int speed;
				std::recursive_mutex filesystem_mutex;
				std::set<FileHandle*> handles;
				static const double readCosts[6];
				static const double seekCosts[6];
				static const double writeCosts[6];
		public:
				static const std::string TYPE;
				FileSystem(Machine* machine);
				void save(YAML::Node&) override;
				void load(YAML::Node) override;
				void close(FileHandle*);
				DMETHOD(getLabel);
				DMETHOD(setLabel);
				DMETHOD(isReadOnly);
				DMETHOD(spaceTotal);
				DMETHOD(spaceUsed);
				DMETHOD(exists);
				DMETHOD(size);
				DMETHOD(isDirectory);
				DMETHOD(lastModified);
				DMETHOD(list);
				DMETHOD(makeDirectory);
				DMETHOD(remove);
				DMETHOD(rename);
				DMETHOD(close);
				DMETHOD(open);
				DMETHOD(read);
				DMETHOD(seek);
				DMETHOD(write);
};

#endif
