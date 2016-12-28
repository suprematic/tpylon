package = "tpylon"
version = "1.0-0"

source = {
   url = "git://github.com/suprematic/tpylon",
   tag = "master"
}

description = {
   summary = "Lua/Torch library for Basler Pylon 5."
   homepage = "git://github.com/suprematic/tpylon"
}

dependencies = {
   "torch >= 7.0",
   "sys >= 1.0",
   "image >= 1.0.1"
}

build = {
   type = "command",
   build_command = [[
cmake -E make_directory build;
cd build;
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(LUA_BINDIR)/.." -DCMAKE_INSTALL_PREFIX="$(PREFIX)"; 
$(MAKE)
   ]],
   install_command = "cd build && $(MAKE) install"
}