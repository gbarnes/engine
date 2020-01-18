#include "stdafx.h"
#include "Layer.h"
#include "Application.h"

namespace Dawn
{
	Layer::Layer(Shared<Dawn::Application> InApplication)
	{
		this->Parent = InApplication;
	}


	Layer::~Layer()
	{
	}
}
