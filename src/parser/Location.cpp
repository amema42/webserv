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

Location::Location(const Location &inst) : type(inst.type), numbers(inst.numbers), l_root(inst.l_root), l_index(inst.l_index), l_methods(inst.l_methods), l_cgi_path(inst.l_cgi_path), l_cgi_extension(inst.l_cgi_extension), l_upload_store(inst.l_upload_store), l_client_max_body_size(inst.l_client_max_body_size)
{
    std::cout << "Location copy constructor" << std::endl;
    return;
}

Location &Location::operator=(const Location &inst)
{
    if (this != &inst) {
        type = inst.type;
        numbers = inst.numbers;
        l_root = inst.l_root;
        l_index = inst.l_index;
        l_methods = inst.l_methods;
        l_cgi_path = inst.l_cgi_path;
        l_cgi_extension = inst.l_cgi_extension;
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

// Implementazione dei setter e getter per i vettori
void Location::addToLRoot(const std::string& value) {
    l_root.push_back(value);
}

std::vector<std::string> Location::getLRoot() const {
    return l_root;
}

void Location::addToLIndex(const std::string& value) {
    l_index.push_back(value);
}

std::vector<std::string> Location::getLIndex() const {
    return l_index;
}

void Location::addToLMethods(const std::string& value) {
    l_methods.push_back(value);
}

std::vector<std::string> Location::getLMethods() const {
    return l_methods;
}

void Location::addToLCgiPath(const std::string& value) {
    l_cgi_path.push_back(value);
}

std::vector<std::string> Location::getLCgiPath() const {
    return l_cgi_path;
}

void Location::addToLCgiExtension(const std::string& value) {
    l_cgi_extension.push_back(value);
}

std::vector<std::string> Location::getLCgiExtension() const {
    return l_cgi_extension;
}

void Location::addToLUploadStore(const std::string& value) {
    l_upload_store.push_back(value);
}

std::vector<std::string> Location::getLUploadStore() const {
    return l_upload_store;
}