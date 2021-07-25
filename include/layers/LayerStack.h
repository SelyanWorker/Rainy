#pragma once

#include "core/Core.h"
#include "Layer.h"

#include <vector>

namespace Rainy {

	class RAINY_API LayerStack
	{
	public:
					LayerStack();
		virtual		~LayerStack();

		void		PushLayer(Layer * layer);

		void		PopLayer();

		std::vector<Layer*>::iterator begin();
		std::vector<Layer*>::iterator end();

	private:
		std::vector<Layer*> m_layers;

	};

}

