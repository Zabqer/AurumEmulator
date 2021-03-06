#include "filesystem.h"

#include "../log.h"
#include "../config.h"
#include "../synchronized.h"

#include <limits>
#include <experimental/filesystem>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>

const std::string FileSystem::TYPE = "filesystem";

const double FileSystem::readCosts[6] = {1 / 1, 1 / 4, 1 / 7, 1 / 10, 1 / 13, 1 / 15};
const double FileSystem::seekCosts[6] = {1 / 1, 1 / 4, 1 / 7, 1 / 10, 1 / 13, 1 / 15};
const double FileSystem::writeCosts[6] = {1 / 1, 1 / 2, 1 / 3, 1 / 4, 1 / 5, 1 / 6};

FileSystem::FileSystem(Machine* machine): Component(machine, TYPE) {
		logC("FileSystem::FileSystem()");
		setMethod("getLabel", {callback: wrapMethod(getLabel), doc: "function():string -- Get the current label of the drive.", direct: true});
		setMethod("setLabel", {callback: wrapMethod(setLabel), doc: "function(value:string):string -- Sets the label of the drive. Returns the new value, which may be truncated."});
		setMethod("isReadOnly", {callback: wrapMethod(isReadOnly), doc: "function():boolean -- Returns whether the file system is read-only.", direct: true});
		setMethod("spaceTotal", {callback: wrapMethod(spaceTotal), doc: "function():number -- The overall capacity of the file system, in bytes.", direct: true});
		setMethod("spaceUsed", {callback: wrapMethod(spaceUsed), doc: "function():number -- The currently used capacity of the file system, in bytes.", direct: true});
		setMethod("exists", {callback: wrapMethod(exists), doc: "function(path:string):boolean -- Returns whether an object exists at the specified absolute path in the file system.", direct: true});
		setMethod("size", {callback: wrapMethod(size), doc: "function(path:string):number -- Returns the size of the object at the specified absolute path in the file system.", direct: true});
		setMethod("isDirectory", {callback: wrapMethod(isDirectory), doc: "function(path:string):boolean -- Returns whether the object at the specified absolute path in the file system is a directory.", direct: true});
		setMethod("lastModified", {callback: wrapMethod(lastModified), doc: "function(path:string):number -- Returns the (real world) timestamp of when the object at the specified absolute path in the file system was modified.", direct: true});
		setMethod("list", {callback: wrapMethod(list), doc: "function(path:string):table -- Returns a list of names of objects in the directory at the specified absolute path in the file system."});
		setMethod("makeDirectory", {callback: wrapMethod(makeDirectory), doc: "function(path:string):boolean -- Creates a directory at the specified absolute path in the file system. Creates parent directories, if necessary."});
		setMethod("remove", {callback: wrapMethod(remove), doc: "function(path:string):boolean -- Removes the object at the specified absolute path in the file system."});
		setMethod("rename", {callback: wrapMethod(rename), doc: "function(from:string, to:string):boolean -- Renames/moves an object from the first specified absolute path in the file system to the second."});
		setMethod("close", {callback: wrapMethod(close), doc: "function(handle:userdata) -- Closes an open file descriptor with the specified handle.", direct: true});
		setMethod("open", {callback: wrapMethod(open), doc: "function(path:string[, mode:string='r']):userdata -- Opens a new file descriptor and returns its handle.", direct: true, limit: 4});
		setMethod("read", {callback: wrapMethod(read), doc: "function(handle:userdata, count:number):string or nil -- Reads up to the specified amount of data from an open file descriptor with the specified handle. Returns nil when EOF is reached.", direct: true, limit: 15});
		setMethod("seek", {callback: wrapMethod(seek), doc: "function(handle:userdata, whence:string, offset:number):number -- Seeks in an open file descriptor with the specified handle. Returns the new pointer position.", direct: true});
		setMethod("write", {callback: wrapMethod(write), doc: "function(handle:userdata, value:string):boolean -- Writes the specified data to an open file descriptor with the specified handle.", direct: true});
		maxTier = 2;
}

FileSystem::FileHandle::FileHandle(FileSystem* fs_, std::string path, std::ios::openmode mode): fs(fs_), stream(path, mode) {
		logC("FileHandle::FileHandle()");
		this->path = path;
}

FileSystem::FileHandle::~FileHandle() {
		logC("FileHandle::~FileHandle()");
		fs->close(this);
}

std::string simplifyPath(std::string path) {
		std::vector<std::string> entries;
		std::string entry;
		unsigned int len = path.size();
		int depth = 0;
		for (unsigned int i = 0; i < len; i++) {
				switch (path[i]) {
						case '/':
								if (entry.size() > 0) {
										entries.push_back(entry);
										entry.clear();
										depth++;
								}
								break;
						case '.':
								if (entry.size() == 0) {
										switch (path[i++]) {
												case '.':
														if (depth > 0) {
															
entries.pop_back();
																depth--;
														} else {
																entries.push_back("..");
														}
										}
										break;
								}
						default:
								entry += path[i];
				}
		}
		if (entry.size() > 0) {
				entries.push_back(entry);
				entry.clear();
		}
		if (!entries.size()) {
				return "/";
		}
		path.clear();
		for (std::string& entry :entries) {
				path += "/" + entry;
		}
		return path;
}

std::string cleanPath(std::string root, std::string apath) {
		std::string path = simplifyPath(apath);
		if (path == "/.." || path.substr(0, 4) == "/../") {
				throw std::runtime_error("file not found");
		}
		return root + path;
}

std::ios::openmode parseMode(std::string mode) {
		if (mode == "r" || mode == "rb") {
				return std::ios::in;
		} else if (mode == "w" || mode == "wb") {
				return std::ios::out;
		} else if (mode == "a" || mode == "ab") {
				return std::ios::app;
		} else {
				throw std::invalid_argument("unsupported mode");
		}
}

FileSystem::FileHandle* checkHandle(Arguments& args, unsigned int index) {
		std::any val = args.checkAny(index);
		if (val.type() == typeid(Userdata*)) {
				FileSystem::FileHandle* handle = dynamic_cast<FileSystem::FileHandle*>(std::any_cast<Userdata*>(val));
				if (handle) {
						return handle;
				}
		}
		/*if (args.checkAny(index).type() == typeid(FileSystem::FileHandle*)) {
				return std::any_cast<FileSystem::FileHandle*>(args.checkAny(index));
		}*/
		throw std::runtime_error("bad file descriptor");
}

void FileSystem::save(YAML::Node& node) {
		logC("FileSystem::save()");
		Component::save(node);
		if (label) {
				node["label"] = *label;
		}
		node["read-only"] = ro;
}

void FileSystem::load(YAML::Node node) {
		logC("FileSystem::load()");
		Component::load(node);
		root = AurumConfig.envPath + "/" + _address + "/";
		if (access(root.c_str(), F_OK) == -1) {
				mkdir(root.c_str(), 777);
		}
		speed = _tier > 0 ? _tier + 1 : _tier;
		_spaceTotal = AurumConfig.hddSizes[_tier] * 1024;
		try {
				label = node["label"].as<std::string>();
		} catch (...) {}
		ro = node["read-only"].as<bool>(false);
}

void FileSystem::close(FileHandle* handle) {
		handles.erase(handles.find(handle));
		handle->stream.close();
}

METHOD(FileSystem, getLabel) {
		logC("FileSystem::getLabel()");
		synchronized(filesystem_mutex);
		if (label) {
				return {*label};
		} else {
				return {Null()};
		}
}

METHOD(FileSystem, setLabel) {
		logC("FileSystem::setLabel()");
		synchronized(filesystem_mutex);
		if (args.checkAny(0).type() == typeid(Null)) {
				label.reset();
				return {Null()};
		} else {
				label = args.checkString(0).substr(0, 16);
				return {*label};
		}
}

METHOD(FileSystem, isReadOnly) {
		logC("FileSystem::isReadOnly()");
		synchronized(filesystem_mutex);
		return {ro};
}

METHOD(FileSystem, spaceTotal) {
		logC("FileSystem::spaceTotal()");
		synchronized(filesystem_mutex);
		if (_spaceTotal == -1) {
				return {std::numeric_limits<double>::max()};
		} else {
				return {_spaceTotal};
		}
}

METHOD(FileSystem, spaceUsed) {
		logC("FileSystem::spaceUsed()");
		synchronized(filesystem_mutex);
		return {_spaceUsed};
}

METHOD(FileSystem, exists) {
		logC("FileSystem::exists()");
		synchronized(filesystem_mutex);
		return {access(cleanPath(root, args.checkString(0)).c_str(), F_OK) == 0};
}

METHOD(FileSystem, size) {
		logC("FileSystem::size()");
		synchronized(filesystem_mutex);
		struct stat st;
		stat(cleanPath(root, args.checkString(0)).c_str(), &st);
		return {st.st_size};
}

METHOD(FileSystem, isDirectory) {
		logC("FileSystem::isDirectory()");
		synchronized(filesystem_mutex);
		struct stat st;
		stat(cleanPath(root, args.checkString(0)).c_str(), &st);
		return {(bool) S_ISDIR(st.st_mode)};
}

METHOD(FileSystem, lastModified) {
		logC("FileSystem::lastModified()");
		synchronized(filesystem_mutex);
		struct stat st;
		stat(cleanPath(root, args.checkString(0)).c_str(), &st);
		return {st.st_mtime};
}

METHOD(FileSystem, list) {
		logC("FileSystem::list()");
		synchronized(filesystem_mutex);
		std::string path = cleanPath(root, args.checkString(0));
		std::vector<std::any> entries;
		struct dirent* dir;
		DIR* d = opendir(path.c_str());
		if (d) {
				while ((dir = readdir(d)) != NULL) {
					if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) {
		 				entries.push_back(std::string(dir->d_name));
					}
				}
				closedir(d);
		} else {
				return {Null()};
		}
		return {entries};
}

METHOD(FileSystem, makeDirectory) {
		logC("FileSystem::makeDirectory()");
		synchronized(filesystem_mutex);
		return {(bool) mkdir(cleanPath(root, args.checkString(0)).c_str(), 777)};
}

METHOD(FileSystem, remove) {
		logC("FileSystem::remove()");
		synchronized(filesystem_mutex);
		logE("FS REMOVE");
		abort();
		//boost::system::error_code ec;
		//boost::filesystem::remove_all(cleanPath(root, args.checkString(0)), ec);
		//return {!ec};
}

METHOD(FileSystem, rename) {
		logC("FileSystem::rename()");
		synchronized(filesystem_mutex);
		logE("FS RENAME");
		abort();
		//boost::system::error_code ec;
		//boost::filesystem::rename(cleanPath(root, args.checkString(0)), cleanPath(root, args.checkString(1)), ec);
		//return {!ec};
}

METHOD(FileSystem, close) {
		logC("FileSystem::close()");
		synchronized(filesystem_mutex);
		FileHandle* handle = checkHandle(args, 0);
		if (!handles.count(handle)) {
				throw std::runtime_error("bad file descriptor");
		}
		close(handle);
		return {true};
}

METHOD(FileSystem, open) {
		logC("FileSystem::open()");
		synchronized(filesystem_mutex);
		if (handles.size() >= AurumConfig.maxHandles) {
				throw std::runtime_error("too many open handles");
		}
		std::string path = cleanPath(root, args.checkString(0));
		std::ios::openmode mode = parseMode(args.optString(1, "r"));
		if (ro && (mode == std::ios::out ||mode == std::ios::app)) {
				throw std::runtime_error("read-only filesystem");
		}
		struct stat st;
		stat(path.c_str(), &st);
		if (mode == std::ios::in && (access(path.c_str(), F_OK) == -1 || S_ISDIR(st.st_mode))) {
				throw std::runtime_error("file not found");
		}
		FileHandle* handle = new FileHandle(this, path, mode);
		if (handle->stream.is_open()) {
				handles.insert(handle);
		}
		return {(Userdata*)handle};
}

METHOD(FileSystem, read) {
		logC("FileSystem::read()");
		synchronized(filesystem_mutex);
		context.consumeCallBudget(readCosts[speed]);
		FileHandle* handle = checkHandle(args, 0);
		if (!handles.count(handle)) {
				throw std::runtime_error("bad file descriptor");
		}
		logD(handle->path);
		size_t cur = handle->stream.tellg();
		handle->stream.seekg(0, std::ios::end);
		size_t remain = ((size_t) handle->stream.tellg()) - cur;
		handle->stream.seekg(cur, std::ios::beg);
		size_t n = std::min<size_t>({remain, (unsigned int) AurumConfig.maxReadBuffer, std::max<size_t>(0, args.checkInteger(1))});
		if (remain == 0 || handle->stream.eof()) {
				return {Null()};
		}
		if (!context.tryChangeBuffer(-AurumConfig.hddReadCost * n)) {
				throw new std::runtime_error("not enough energy");
		}
		char* buffer = new char[n];
		handle->stream.read(buffer, n);
		std::string result(buffer, n);
		delete[] buffer;
		return {result};
}

METHOD(FileSystem, seek) {
		logC("FileSystem::seek()");
		synchronized(filesystem_mutex);
		context.consumeCallBudget(seekCosts[speed]);
		FileHandle* handle = checkHandle(args, 0);
		if (!handles.count(handle)) {
				throw std::runtime_error("bad file descriptor");
		}
		std::string whence = args.checkString(1);
		int offset = args.checkInteger(2);
		long long int cur = handle->stream.tellg();
		handle->stream.seekg(0, std::ios::end);
		size_t size = handle->stream.tellg();
		handle->stream.seekg(cur, std::ios::beg);
		if (whence == "cur") {
				handle->stream.seekg(std::max<long long int>(std::min<long long int>(offset + cur, size), 0), std::ios::beg);
		} else if (whence == "set") {
				handle->stream.seekg(std::max<long long int>(std::min<long long int>(offset, size), 0), std::ios::beg);
		} else if (whence == "end") {
				handle->stream.seekg(std::min<long long int>(std::min<long long int>(size + offset, size), 0), std::ios::beg);
		} else {
				throw std::invalid_argument("invalid mode");
		}
		return {(size_t) handle->stream.tellg()};
}

METHOD(FileSystem, write) {
		logC("FileSystem::write()");
		synchronized(filesystem_mutex);
		context.consumeCallBudget(writeCosts[speed]);
		FileHandle* handle = checkHandle(args, 0);
		if (!handles.count(handle)) {
				throw std::runtime_error("bad file descriptor");
		}
		std::string data = args.checkString(1);
		if (!context.tryChangeBuffer(-AurumConfig.hddWriteCost * data.length())) {
				throw new std::runtime_error("not enough energy");
		}
		handle->stream.write(data.c_str(), data.length());
		return {true};
}
