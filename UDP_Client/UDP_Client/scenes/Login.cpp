#include "Login.h"
#include "../ImageObject.h"
#include "../Spawner.h"
#include "../Managers/SceneManager.h"
#include <iostream>
#include "../Managers/InputManager.h"
#include "../Managers/PacketManager.h"

void Login::OnEnter() {
	// Fondo de pantalla
	Object* background = new ImageObject("resources/Login/Fondo.png", Vector2{0, 0}, Vector2{1536, 1024});
	background->GetTransform()->position = { RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f };
	background->GetTransform()->scale = { 1.0f, 1.0f };
	SPAWN.SpawnObject(background);

	// Texto usuario
	usernameTextButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Login/user-button.png");
	usernameTextButton->SetAction([this]() {
		StartEditingUsername();
		});
	usernameTextButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	usernameTextButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f - 100.0f);
	usernameTextButton->GetBackground()->GetTransform()->scale = Vector2(32.0f, 6.0f);
	usernameTextButton->GetBackground()->GetTransform()->position = usernameTextButton->GetTransform()->position;
	SPAWN.SpawnObject(usernameTextButton->GetBackground());
	SPAWN.SpawnObject(usernameTextButton);

	// Texto contraseña
	passwordTextButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Login/Password-button.png");
	passwordTextButton->SetAction([this]() {
		StartEditingPassword();
		});
	passwordTextButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	passwordTextButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f);
	passwordTextButton->GetBackground()->GetTransform()->scale = Vector2(32.0f, 6.0f);
	passwordTextButton->GetBackground()->GetTransform()->position = passwordTextButton->GetTransform()->position;
	SPAWN.SpawnObject(passwordTextButton->GetBackground());
	SPAWN.SpawnObject(passwordTextButton);

	// Botón de login
	Button* loginButton = new Button("", sf::Color{18, 113, 230}, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Login/login-button.png");
	loginButton->SetAction([this]() {
		std::cout << "LOGIN:" << username << "," << password << std::endl;
		LoginAction();
		});
	loginButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	loginButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f + 100);
	loginButton->GetBackground()->GetTransform()->scale = Vector2(25.0f, 6.0f);
	loginButton->GetBackground()->GetTransform()->position = loginButton->GetTransform()->position;
	SPAWN.SpawnObject(loginButton->GetBackground());
	SPAWN.SpawnObject(loginButton);

	// Botón de registro
	Button* registerButton = new Button("", sf::Color{ 18, 113, 230 }, Vector2(0.0f, 0.0f), Vector2(16.0f, 16.0f), "resources/Login/register-button.png");
	registerButton->SetAction([this]() {
		std::cout << "REGISTER" << std::endl;
		RegisterAction();
		});
	registerButton->GetTransform()->scale = Vector2(1.0f, 1.0f);
	registerButton->GetTransform()->position = Vector2(RM->WINDOW_WIDTH / 2.0f, RM->WINDOW_HEIGHT / 2.0f + 200);
	registerButton->GetBackground()->GetTransform()->scale = Vector2(25.0f, 6.0f);
	registerButton->GetBackground()->GetTransform()->position = registerButton->GetTransform()->position;
	SPAWN.SpawnObject(registerButton->GetBackground());
	SPAWN.SpawnObject(registerButton);

	Input.BeginTextInput();
}

void Login::OnExit() {
	Scene::OnExit();
	Input.EndTextInput();
}

void Login::Update() {
	Scene::Update();

	// Detectar el texto al cambiar
	if (activeField == ActiveField::Username) {
		username = Input.GetInputText();
	}

	if (activeField == ActiveField::Password) {
		password = Input.GetInputText();
	}

	// Actualizar texto
	if (usernameTextButton) {
		usernameTextButton->SetText(username.empty() ? "Username" : username);
	}

	if (passwordTextButton) {
		passwordTextButton->SetText(password.empty() ? "Password" : password);
	}
}

void Login::Render() {
	Scene::Render();
}

void Login::StartEditingUsername() {
	if (activeField == ActiveField::Password) {
		password = Input.GetInputText();
	}

	activeField = ActiveField::Username;
	Input.SetInputText(username);
}

void Login::StartEditingPassword() {
	if (activeField == ActiveField::Username) {
		username = Input.GetInputText();
	}

	activeField = ActiveField::Password;
	Input.SetInputText(password);
}

void Login::LoginAction() {
	PM->SendLoginRequest(username, password);
}

void Login::RegisterAction() {
	PM->SendRegisterRequest(username, password);
}