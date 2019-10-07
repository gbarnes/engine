#include "Layer.h"
#include "Application.h"

namespace Dawn
{
	Layer::Layer(Shared<Dawn::Application> InApplication)
	{
		this->Application = InApplication;
	}


	Layer::~Layer()
	{
	}
}
