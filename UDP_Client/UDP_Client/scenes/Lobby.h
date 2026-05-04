	#pragma once
#include "../Scene.h"
#include "../MenuElements/Button.h"

class Lobby : public Scene
{
private:
	enum class ActiveField {
		None,
		IdLobby
	};

	ActiveField activeField = ActiveField::None;

	// ID de la sala ingresado por el usuario
	std::string idLobby = "";

	// Botones para unirte y crear sala + ranking
	Button* idLobbyTextButton;
	Button* createButton;       
	Button* joinButton;       
	Button* rankingButton;   

	void StartEditingIdLobby();

	// Acciones de los botones
	void CreateAction();        
	void JoinAction();         
	void RankingSceneAction();  

public:
	Lobby() = default;
	void Update() override;
	void OnEnter() override;
	void OnExit() override;
	void Render() override;
};

