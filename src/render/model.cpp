#include "model.h"
#include "core/components.h"
namespace FlatEngine {
	Model::Model(std::string const& path, bool gamma) : gammaCorrection(gamma) {
		loadModel(path);
	}
	void Model::Draw(Shader& shader, const MeshRendererComponent& mrc) {
		for(unsigned int i = 0; i < meshes.size(); i++){
			meshes[i].colors[i] = mrc.colors[i];
			meshes[i].Draw(shader);
		}
	}
}