#include <iostream>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

int main(int argc, char* argv[])
{
   path Desktop("C://Desktop");
   recursive_directory_iterator end;

   for(recursive_directory_iterator i(Desktop); i != end; ++i)
   {
       path file_name = *i;
       std::cout << file_name.string() << std::endl;
   }
   return 0;
}