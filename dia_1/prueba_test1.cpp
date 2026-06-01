#include <iostream>
void funcionCuriosa()
{
static int x=0;
std::cout << x++ << std::endl;;
}
int main()
{
	funcionCuriosa();
	funcionCuriosa();
	funcionCuriosa();
}