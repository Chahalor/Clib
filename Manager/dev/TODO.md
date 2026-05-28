# Modules Manager

## setup
- [X] update the CMakeLists.txt based on the wanted modules
	- Main CMakeLists
	- Modules CMakeLists
- [X] the current removal of module remove the internal implementation of each modules, should not do this

- [ ] create a cache dir that will be the location of the git repo
	- from here we will copy the wanted modules
	- update will be done with a git pull in the cache dir
	- this dir will be place on the ~ or to the dir specified in the CLI
	- that allow to add new module after and not having to reclone the full lib

- [ ] fix the standards dir include using the generated CMakeLists.txt (-Istandards in every cc)