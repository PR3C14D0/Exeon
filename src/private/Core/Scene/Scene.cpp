#include "Core/Scene/Scene.h"

Scene::Scene(std::string name) {
	this->name = name;
	this->m_go = new GameObject("Bistro");
}

void Scene::Init() {
	Mesh* m_mesh = new Mesh("StaticMeshComponent", &m_go->transform);
	m_go->m_components.push_back(m_mesh);
	m_mesh->LoadModel("DamagedHelmet.glb");
	this->AddGameObject(m_go);

	this->m_editorCamera = new EditorCamera("EditorCamera");
	this->AddGameObject(this->m_editorCamera);

	this->SetCurrentCamera("EditorCamera");

	Light* sampleLight = new Light("Sample Light");
	this->AddGameObject(sampleLight);

	for (std::pair<std::string, GameObject*> object : this->m_gameObjects) {
		object.second->Init();
	}
	// m_go->transform.Rotate(Vector3(90.f, 0.f, 0.f));
}

void Scene::ShadowPass() {
	std::vector<Light*> lights = this->GetLights();

	for (Light* light : lights) {
		light->Render();
		WVP wvp = light->m_wvp;
		for (std::pair<std::string, GameObject*> object : this->m_gameObjects) {
			/* Check if object is a Light, if it is, skip it. */
			if (dynamic_cast<Light*>(object.second)) continue;
			object.second->ShadowPass(wvp);
		}
	}
}

void Scene::Update() {
	for (std::pair<std::string, GameObject*> object : this->m_gameObjects) {
		object.second->Update();
	}
}

void Scene::Render() {
	for (std::pair<std::string, GameObject*> object : this->m_gameObjects) {
		/* Check if object is a Light, if it is, skip it. */
		if (dynamic_cast<Light*>(object.second)) continue;
		object.second->Render();
	}
}

bool Scene::ObjectExists(std::string name) {
	if (this->m_gameObjects.size() > 0) {
		if (this->m_gameObjects.count(name) > 0)
			return true;
		spdlog::error("Scene#{0}: GameObject {1} not found in the scene.", this->name, name);
	}
	else {
		spdlog::error("Scene#{0}: No GameObjects added on the Scene.", this->name);
	}

	return false;
}

bool Scene::CameraExists(std::string name) {
	if (this->m_cameras.size() > 0) {
		if (this->m_cameras.count(name) > 0)
			return true;
		spdlog::error("Scene#{0}: Camera {1} not found in the scene.", this->name, name);
	}
	else {
		spdlog::error("Scene#{0}: No Cameras added on the Scene.", this->name);
	}

	return false;
}

void Scene::SetCurrentCamera(std::string cameraName) {
	if (!this->CameraExists(cameraName)) {
		spdlog::error("Scene#{0}: No Camera type GameObjects named {1} exist in the scene.", this->name, cameraName);
		return;
	}

	this->m_currentCamera = this->m_cameras[cameraName];
}

Camera* Scene::GetCurrentCamera() {
	return this->m_currentCamera;
}
GameObject* Scene::GetObject(std::string name) {
	if (!this->ObjectExists(name)) {
		spdlog::error("Scene::GetObject: No GameObject with name {0} found", name);
		return nullptr;
	}

	return this->m_gameObjects[name];
}

std::vector<Light *> Scene::GetLights() {
	std::vector<Light*> lights;

	for (std::pair<std::string, Light*> light : this->m_lights) {
		lights.push_back(light.second);
	}

	return lights;
}


void Scene::AddGameObject(GameObject* object) {
	if (!object)
		return;

	if (this->ObjectExists(object->m_name)) {
		spdlog::error("Scene#{0}: A GameObject with the name {1} already exists in the scene.", this->name, object->m_name);
		return;
	}

	if (Camera* cam = dynamic_cast<Camera*>(object))
		this->m_cameras[object->m_name] = cam;

	if (Light* light = dynamic_cast<Light*>(object))
		this->m_lights[object->m_name] = light;

	this->m_gameObjects[object->m_name] = object;
}