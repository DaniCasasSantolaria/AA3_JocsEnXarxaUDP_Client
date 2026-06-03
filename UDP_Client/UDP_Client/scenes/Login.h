#pragma once
#include "../Scene.h"
#include "../MenuElements/Button.h"

class Login : public Scene {
private:
	enum class ActiveField {
		None,	
		Username,
		Password
	};

	ActiveField activeField = ActiveField::None;

	std::string username = "";
	std::string password = "";

	// Botones para usuario y contraseña
	Button* usernameTextButton;
	Button* passwordTextButton;

	void StartEditingUsername(); 
	void StartEditingPassword();

	// Acciones de los botones
	void LoginAction();
	void RegisterAction();

public:
	Login() = default;
	void Update() override;
	void OnEnter() override;
	void OnExit() override;
	void Render() override;
};