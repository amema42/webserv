#include "../../include/webServ.hpp"

Location::Location()
{
    std::cout << "Location default constructor" << std::endl;
    numbers.push_back(L_ROOT_ARG);
    numbers.push_back(L_INDEX_ARG);
    numbers.push_back(L_METODS_ARG);
    numbers.push_back(L_CGI_PATH_ARG);
    numbers.push_back(L_CGI_EXTENSION_ARG);
    numbers.push_back(L_UPLOAD_STORE_ARG);
    numbers.push_back(L_CLIENT_MAX_BODY_SIZE);
}

Location::~Location()
{
    std::cout << "Location destructor" << std::endl;
    return;
}

Location::Location(const Location &inst) : type(inst.type), numbers(inst.numbers), path(inst.path), l_root(inst.l_root), l_index(inst.l_index), l_methods(inst.l_methods), l_upload_store(inst.l_upload_store), l_client_max_body_size(inst.l_client_max_body_size)
{
    std::cout << "Location copy constructor" << std::endl;
    return;
}

Location &Location::operator=(const Location &inst)
{
    if (this != &inst) 
	{
        type = inst.type;
        type = inst.type;
        numbers = inst.numbers;
        path = inst.path;
        l_root = inst.l_root;
        l_index = inst.l_index;
        l_methods = inst.l_methods;
        l_upload_store = inst.l_upload_store;
        l_client_max_body_size = inst.l_client_max_body_size;
    }
    return *this;
}

bool Location::setNumberToZero(int number) 
{
    std::vector<int>::iterator it = std::find(numbers.begin(), numbers.end(), number);
    if (it != numbers.end()) {
        *it = 0;
        return true;
    }
    return false;
}

void Location::addToLMethods(const std::string& value) {
    l_methods.push_back(value);
}
