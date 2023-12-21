#pragma once

class Renderer {
public:
	explicit Renderer() = default;
	virtual void Init();
	virtual void Update();
};