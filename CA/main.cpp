#include <SFML/Graphics.hpp>
#include <string>
#include "json.hpp"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>

class Textbox {
public:
	int x, y, width, height;
	sf::RectangleShape  background;
	sf::Text text;
	std::string displayText;
	sf::Text backgroundText;

	int displayTextInt() {
		std::cout << "Converting: " << displayText << " to int" << std::endl;
		return std::stoi(displayText);

	}

	Textbox(int inputX, int inputY, int inputWidth, int inputHeight, std::string bgtext, sf::Font* font) {
		x = inputX;
		y = inputY;
		width = inputWidth;
		height = inputHeight;
		background.setPosition(x, y);
		background.setSize(sf::Vector2f(width, height));
		background.setOutlineColor(sf::Color::Black);
		background.setFillColor(sf::Color(28, 41, 27));
		background.setOutlineThickness(2);

		text.setFont(*font);
		text.setCharacterSize(inputHeight);
		text.setFillColor(sf::Color::White);
		text.setPosition(inputX, inputY);
		text.setString(displayText);

		backgroundText.setFont(*font);
		backgroundText.setCharacterSize(inputHeight);
		backgroundText.setFillColor(sf::Color(28 - 5, 41 - 5, 27 - 5));
		backgroundText.setPosition(inputX, inputY);
		backgroundText.setString(bgtext);
	}

	Textbox() {
		x = 0;
		y = 0;
		width = 0;
		height = 0;
	}

	void draw(sf::RenderWindow& window) {
		window.draw(background);
		window.draw(backgroundText);
		window.draw(text);
	}

	void setDisplayText(std::string text) {
		displayText = text;
		UpdateDisplayText();
	}

	void UpdateDisplayText() {
		text.setString(displayText);
	}
};



class TileMap : public sf::Drawable, public sf::Transformable
{
public:

	bool load(const std::string& tileset, sf::Vector2u tileSize, std::vector<int> tiles, unsigned int width, unsigned int height)
	{
		// load the tileset texture
		if (!m_tileset.loadFromFile(tileset))
			return false;

		// resize the vertex array to fit the level size
		m_vertices.setPrimitiveType(sf::Quads);
		m_vertices.resize(width * height * 4);

		// populate the vertex array, with one quad per tile
		for (unsigned int i = 0; i < width; ++i)
			for (unsigned int j = 0; j < height; ++j)
			{
				// get the current tile number
				int tileNumber = tiles[i + j * width];

				// find its position in the tileset texture
				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				// get a pointer to the current tile's quad
				sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

				// define its 4 corners
				quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				// define its 4 texture coordinates
				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}

		return true;
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// apply the tileset texture
		states.texture = &m_tileset;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
};


class Tile {

public:
	int textureValue = 0;
	int x = 0;
	int y = 0;
	sf::Sprite sprite = sf::Sprite();

	Tile(int tValue, int xpos, int ypos, std::vector<sf::Texture> &textures) {
		textureValue = tValue;
		x = xpos;
		y = ypos;
		sprite = sf::Sprite(textures[tValue]);
		sprite.setPosition(x, y);
	}

	void UpdateTexture(int tValue, std::vector<sf::Texture> &textures) {
		textureValue = tValue;
		sprite.setTexture(textures[tValue]);
	}
	Tile() {
		textureValue = 0;
		x = 0;
		y = 0;
		sprite = sf::Sprite();
	}

};



namespace ns {
	struct MapForJsonStruct {
		std::vector<int> map;
		//ToDo
		std::vector<int> tileSize, mapSize; //what structure would suit these. Note SFML types wont work here.
		std::string textureForMap;
		const int* get_level()
		{
			//since vector must store data contiguously, the following works for array
			int* a = &map[0];
			return a;
		}
	};

	void to_json(nlohmann::json& j, const MapForJsonStruct& ms) {
		j = nlohmann::json{
			{"map",ms.map},
			{"tilesize",ms.tileSize},
			{"mapsize",ms.mapSize},
			{"maptexture",ms.textureForMap}
		};
	}
	void from_json(const nlohmann::json& j, MapForJsonStruct& ms) {
		j.at("map").get_to(ms.map);
		j.at("tilesize").get_to(ms.tileSize);
		j.at("mapsize").get_to(ms.mapSize);
		j.at("maptexture").get_to(ms.textureForMap);
	}

}


void SaveToJson(std::string workingFile, std::vector<int> level, int tilesizeX, int tilesizeY, int mapsize, std::string tileset) {


	/*ns::MapForJsonStruct map;
	map.map = level;
	map.tileSize = {tilesizeX,tilesizeY};
	map.mapSize = {mapsize,mapsize};
	map.textureForMap = tileset;
	nlohmann::json j = map*/


	/*nlohmann::json levelData;

	levelData["level"] = level;
	levelData["tileset"] = tileset;
	levelData["tileSize"] = {tilesizeX,tilesizeY};
	levelData["MapSize"] = {mapsize,mapsize};

	std::ofstream outFile("map.json");
	outFile << levelData;
	outFile.close();

	*/


	std::ofstream outJson(workingFile+".json");
	sf::Vector2u widthAndHeight(mapsize, mapsize), tileSize(tilesizeX, tilesizeY);
	nlohmann::json jsonMap = {
		{"map",level},
		{"tilesize",{tileSize.x,tileSize.y}},
		{"mapsize",{widthAndHeight.x,widthAndHeight.y}},
		{"maptexture",tileset}
	};
	outJson << jsonMap << std::endl;
	outJson.close();

}

void LoadFromJson(std::string workingFile, std::vector<int>& level, int& tilesizeX, int& tilesizeY, int& mapsize, std::string& tileset) {

	nlohmann::json j2;
	std::ifstream inputFile(workingFile+".json");

	if (!inputFile.good()) {
		level = {};
		tileset = "";
		mapsize = 0;
		tilesizeX = 0;
		tilesizeY = 0;
		return;
	}

	inputFile >> j2;
	inputFile.close();


	const std::string& load_tileset(j2["maptexture"]);
	std::vector<int> load_tiles = j2["map"];
	int load_mapWidth = j2["mapsize"][0];
	int load_mapHeight = j2["mapsize"][1];
	std::vector<int> load_tileSize = j2["tilesize"];


	std::cout << "J2" << j2 << std::endl;

	/*
	for(int i: load_tileSize){
		std::cout << i << std::endl;
	}
	for(int i : load_tiles){
		std::cout << i << std::endl;
	}
	std::cout << "load_tileset: " << load_tileset  << " load_mapWidth: " << load_mapWidth << " load_mapHeight: " << load_mapHeight << std::endl;
	*/

	level = load_tiles;
	tileset = load_tileset;
	if (load_mapWidth > load_mapHeight) {
		mapsize = load_mapWidth;
	}
	else {
		mapsize = load_mapHeight;
	}
	tilesizeX = load_tileSize[0];
	tilesizeY = load_tileSize[1];

}



void InitializeNumOfTextures(std::string tileset, int& numOfTexturesX, int& numOfTexturesY, int& numOfTextures, int tilesizeX, int tilesizeY) {
	sf::Texture m_tileset;
	if (!m_tileset.loadFromFile(tileset)) {
		std::cout << "Failed To Load Tileset " << tileset << std::endl;
		numOfTexturesX = 0;
		numOfTexturesY = 0;
		numOfTextures = 0;
		return;
	}


	numOfTexturesX = m_tileset.getSize().x / tilesizeX;

	numOfTexturesY = m_tileset.getSize().y / tilesizeY;

	numOfTextures = numOfTexturesX * numOfTexturesY;
}

void CreateSpriteSheet(std::vector <sf::Texture>& textures, std::string tileset, int numOfTexturesX, int numOfTexturesY, int tilesizeX, int tilesizeY) {
	for (int y = 0; y < numOfTexturesY; y++) {
		for (int x = 0; x < numOfTexturesX; x++) {
			sf::Texture textureToAdd;
			textureToAdd.loadFromFile(tileset, sf::IntRect((tilesizeX * x), (tilesizeY * y), tilesizeX, tilesizeY));

			textures[x % numOfTexturesX + y * numOfTexturesX] = (textureToAdd);
		}
	}
}

void SetTileSprites(std::vector <Tile> &tiles, std::vector<int> level, int mapsize, int tilesizeX, int tilesizeY, std::vector<sf::Texture> &textures) {
	for (int y = 0; y < mapsize; y++) {
		for (int x = 0; x < mapsize; x++) {
			//std::cout << "Setting sprite "<< (x+(y*mapsize)) << " to position :" << x << ", " << y << std::endl;
			tiles[x + (y * mapsize)] = Tile(level[x + (y * mapsize)], x * tilesizeX, y * tilesizeY, textures);
		}
	}
	std::cout << "Tiles size: " << tiles.size() << std::endl;
}

void createPreviewTiles(std::vector<Tile> &previewTiles, std::vector <sf::Texture> &textures, int tilesizeX, int tilesizeY, int numOfTextures) {
	int yOffset = 0;
	int xOffset = 0;
	int tileBreakAt = 0;
	if (tilesizeX != 0) {
		tileBreakAt = (270 / tilesizeX) - 1;
		std::cout << "Breaking at " << " 270/" << tilesizeX << " = " << (270 / tilesizeX) - 1 << std::endl;
	}
	std::cout << "NumTextures is " << numOfTextures << std::endl;

	for (int i = 0; i < numOfTextures; i++) {
		//std::cout << "In Loop itteration: " << i << std::endl;

		if ((i + 1) % tileBreakAt == 0) {
			yOffset++;
		}
		if (xOffset > tileBreakAt) {
			xOffset = 0;
		}
		//std::cout << "Setting preview Tiles at " << i << std::endl;

		previewTiles[i] = Tile(i, 830 + xOffset * tilesizeX, (yOffset * tilesizeY) + 35, textures);
		xOffset++;
	}

}

void CreateTilemap(std::string& oldWorkingFile, std::string newWorkingFile, std::vector<int>& level, int& oldMapsize, int newMapsize, int& oldTilesizeX, int newTilesizeX, int& oldTilesizeY, int newTilesizeY, std::string& oldTileset, std::string newTileset) {
	oldWorkingFile = newWorkingFile;
	std::cout << "Passed working File" << std::endl;

	level.resize(newMapsize * newMapsize);
	std::cout << "Passed level Resize" << std::endl;

	oldTileset = newTileset;
	std::cout << "Passed tileset" << std::endl;

	oldMapsize = newMapsize;
	std::cout << "Passed mapSize" << std::endl;

	oldTilesizeX = newTilesizeX;
	std::cout << "Passed tileSizeX" << std::endl;

	oldTilesizeY = newTilesizeY;
	std::cout << "Passed tileSizeY" << std::endl;
}

int main()
{

	sf::Font font;
	if (!font.loadFromFile("sansation.ttf"))
	{

	}
	sf::RenderWindow window(sf::VideoMode(1080, 720), "Level Editor");
	window.setVerticalSyncEnabled(true);

	std::string tileset = "";
	int tilesizeX = 0;
	int tilesizeY = 0;
	int mapsize = 0;
	std::vector<int> level;

	std::string workingFile;

	//std::cout << "Enter workfile location: ";
	//std::cin >> workingFile;
	workingFile = " ";
	std::cout << "Passed Text enter" << std::endl;

	LoadFromJson(workingFile, level, tilesizeX, tilesizeY, mapsize, tileset);

	std::cout << "Passed Loading" << std::endl;
	/*
	std::cout << "Level" <<std::endl;
	for(int i : level){
		std::cout << i <<std::endl;
	}

	std::cout << "Enter texture name: ";
	std::cin >> tileset;

	std::cout << "Enter tilesizeX: ";
	std::cin >> tilesizeX;


	std::cout << "Enter tilesizeY: ";
	std::cin >> tilesizeY;

	std::cout << "Enter size of map : ";
	std::cin >> mapsize;
	*/

	int numOfTexturesX;
	int numOfTexturesY;
	int numOfTextures;

	InitializeNumOfTextures(tileset, numOfTexturesX, numOfTexturesY, numOfTextures, tilesizeX, tilesizeY);

	std::cout << "Passed num of Texture init" << std::endl;


	std::vector<sf::Texture> textures(numOfTextures);

	/*std::cout << "numOfTexturesX:  "<< numOfTexturesX << std::endl;
	std::cout << "numOfTexturesY:  "<< numOfTexturesY << std::endl;
	std::cout << "numOfTextures:  "<< numOfTextures << std::endl;
	*/

	/*
	for(int y = 0 ; y < numOfTexturesY ; y++){
		for(int x = 0 ; x < numOfTexturesX ; x++){
			sf::Texture textureToAdd;
			textureToAdd.loadFromFile(tileset, sf::IntRect((tilesizeX*x), (tilesizeY*y), tilesizeX, tilesizeY));

			textures[x%numOfTexturesX+y*numOfTexturesX] = (textureToAdd);
		}
	}
	*/

	CreateSpriteSheet(textures, tileset, numOfTexturesX, numOfTexturesY, tilesizeX, tilesizeY);
	std::cout << "Passed spriteSheet" << std::endl;

	std::vector<Tile> tiles(mapsize * mapsize);
	std::cout << "Array Size is : " << mapsize * mapsize << std::endl;

	/*
	srand (10);

	for(int i = 0 ; i < mapsize*mapsize; i++){
		level[i] = rand() % numOfTextures;
	}

	*/
	/*
	for(int y = 0 ; y < mapsize ; y++){
		for(int x = 0 ; x < mapsize ; x++){
			//std::cout << "Setting sprite "<< (x+(y*mapsize)) << " to position :" << x << ", " << y << std::endl;
			tiles[x+(y*mapsize)] = Tile(level[x+(y*mapsize)],x*tilesizeX,y*tilesizeY,textures);
		}
	}
	*/

	SetTileSprites(tiles, level, mapsize, tilesizeX, tilesizeY, textures);
	std::cout << "Passed SetTileSprites" << std::endl;

	int selectedTileType = 0;
	int selectedTextBox = 0;

	std::vector<Tile> previewTiles(numOfTextures);

	createPreviewTiles(previewTiles, textures, tilesizeX, tilesizeY, numOfTextures);
	/*
	int yOffset = 0;
	int xOffset = 0;
	int tileBreakAt = 0;
	if(tilesizeX!=0){
		tileBreakAt = (270/tilesizeX)-1;
		std::cout << "Breaking at "<< tilesizeX <<"/270 ="<< tilesizeX/270 << std::endl;
	}
	for(int i = 0 ; i < numOfTextures ; i ++){
		if((i+1)%tileBreakAt==0){
			yOffset++;
		}
		if(xOffset > tileBreakAt){
			xOffset = 0;
		}
		previewTiles[i] = Tile(i,830+xOffset*tilesizeX,(yOffset*tilesizeY)+35,textures);
		xOffset++;
	}*/
	std::cout << "Passed setting preview tiles" << std::endl;
	/*
	std::string workingFile,
	std::vector<int> level,
	int tilesizeX,
	int tilesizeY,
	int mapsize,
	std::string tileset)*/



	Textbox boxFileLoc(840, 500, 150, 25, "File Locaton", &font);
	Textbox boxTilesizeX(840, 530, 100, 25, "Tilesize X", &font);
	Textbox boxTilesizeY(945, 530, 100, 25, "Tilesize Y", &font);
	Textbox boxMapsize(840, 560, 125, 25, "Map Size", &font);
	Textbox boxTexture(840, 590, 125, 25, "Texture", &font);

	Textbox textboxes[5];
	textboxes[0] = boxFileLoc;
	textboxes[1] = boxTilesizeX;
	textboxes[2] = boxTilesizeY;
	textboxes[3] = boxMapsize;
	textboxes[4] = boxTexture;

	bool mouseReleased = true;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode == '\b') { // handle backspace explicitly
					if (textboxes[selectedTextBox].displayText.size() > 0) {
						textboxes[selectedTextBox].displayText.erase(textboxes[selectedTextBox].displayText.size() - 1, 1);
					}
				}
				else {
					textboxes[selectedTextBox].displayText += event.text.unicode;
				}
				textboxes[selectedTextBox].UpdateDisplayText();

				std::cout << textboxes[selectedTextBox].displayText << std::endl;
			}


			if (event.type == sf::Event::Closed)
				window.close();
		}



		sf::RectangleShape menuBG;
		menuBG.setSize(sf::Vector2f(270, 720));
		menuBG.setPosition(810, 0);
		sf::Color colorBG = sf::Color(43, 92, 43);
		menuBG.setFillColor(colorBG);


		sf::Text menuTitle;
		menuTitle.setFont(font);
		menuTitle.setFillColor(sf::Color(115, 115, 115));
		menuTitle.setPosition(810 + 30, 30);
		menuTitle.setString("Menu");


		sf::RectangleShape saveToJsonButton;
		saveToJsonButton.setSize(sf::Vector2f(55, 25));
		saveToJsonButton.setOutlineColor(sf::Color::White);
		saveToJsonButton.setOutlineThickness(5);
		saveToJsonButton.setPosition(840, 650);
		saveToJsonButton.setFillColor(sf::Color(0, 175, 0));

		sf::Text saveText;
		saveText.setFont(font);
		saveText.setCharacterSize(24);
		saveText.setFillColor(sf::Color::White);
		saveText.setPosition(840, 650);
		saveText.setString("Save");

		sf::RectangleShape loadFromJsonButton;
		loadFromJsonButton.setSize(sf::Vector2f(55, 25));
		loadFromJsonButton.setOutlineColor(sf::Color::White);
		loadFromJsonButton.setOutlineThickness(5);
		loadFromJsonButton.setPosition(915, 650);
		loadFromJsonButton.setFillColor(sf::Color(0, 175, 0));

		sf::Text loadText;
		loadText.setFont(font);
		loadText.setCharacterSize(24);
		loadText.setFillColor(sf::Color::White);
		loadText.setPosition(915, 650);
		loadText.setString("Load");

		sf::RectangleShape createButton;
		createButton.setSize(sf::Vector2f(55, 25));
		createButton.setOutlineColor(sf::Color::White);
		createButton.setOutlineThickness(5);
		createButton.setPosition(990, 650);
		createButton.setFillColor(sf::Color(0, 175, 0));

		sf::Text createText;
		createText.setFont(font);
		createText.setCharacterSize(24);
		createText.setFillColor(sf::Color::White);
		createText.setPosition(990, 650);
		createText.setString("Create");


		sf::Vector2i localMousePosition = sf::Mouse::getPosition(window);

		if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			mouseReleased = true;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseReleased)
		{
			mouseReleased - false;
			if (localMousePosition.x < 810) {
				std::cout << "Mouse Pressed at: " << localMousePosition.x << ", " << localMousePosition.y << std::endl;
				
				for (int i = 0; i < mapsize * mapsize; i++) {
					if (localMousePosition.x > tiles[i].x and
						localMousePosition.x < tiles[i].x + tilesizeX and
						localMousePosition.y > tiles[i].y and
						localMousePosition.y < tiles[i].y + tilesizeY) {
						std::cout << "Mouse Pressed at: " << localMousePosition.x << ", " << localMousePosition.y << std::endl;
						tiles[i].UpdateTexture(selectedTileType, textures);
						level[i] = selectedTileType;
						
						break;


					}
				}
				
			}
			else if (localMousePosition.x > saveToJsonButton.getPosition().x and
				localMousePosition.x < saveToJsonButton.getPosition().x + saveToJsonButton.getSize().x and
				localMousePosition.y > saveToJsonButton.getPosition().y and
				localMousePosition.y < saveToJsonButton.getPosition().y + saveToJsonButton.getSize().y) {
				std::cout << "Saving Json" << std::endl;
				SaveToJson(textboxes[0].displayText, level, tilesizeX, tilesizeY, mapsize, tileset);





			}
			else if (localMousePosition.x > loadFromJsonButton.getPosition().x and
				localMousePosition.x < loadFromJsonButton.getPosition().x + loadFromJsonButton.getSize().x and
				localMousePosition.y > loadFromJsonButton.getPosition().y and
				localMousePosition.y < loadFromJsonButton.getPosition().y + loadFromJsonButton.getSize().y) {
				std::cout << "Loading Json" << std::endl;
				LoadFromJson(textboxes[0].displayText, level, tilesizeX, tilesizeY, mapsize, tileset);
				InitializeNumOfTextures(tileset, numOfTexturesX, numOfTexturesY, numOfTextures, tilesizeX, tilesizeY);
				textures.resize(numOfTextures);
				CreateSpriteSheet(textures, tileset, numOfTexturesX, numOfTexturesY, tilesizeX, tilesizeY);
				tiles.resize(mapsize * mapsize);
				SetTileSprites(tiles, level, mapsize, tilesizeX, tilesizeY, textures);
				previewTiles.resize(numOfTextures);
				createPreviewTiles(previewTiles, textures, tilesizeX, tilesizeY, numOfTextures);
				SetTileSprites(tiles, level, mapsize, tilesizeX, tilesizeY, textures);


			}
			else if (localMousePosition.x > createButton.getPosition().x and
				localMousePosition.x < createButton.getPosition().x + createButton.getSize().x and
				localMousePosition.y > createButton.getPosition().y and
				localMousePosition.y < createButton.getPosition().y + createButton.getSize().y) {
				std::cout << "Creating Tilemap" << std::endl;
				/*createTilemap(std::string &oldWorkingFile, std::string newWorkingFile,
				std::vector<int>& level,
				int &oldMapsize,int newMapsize,
				int& oldtilesizeX,int newTilesizeX,
				int& oldTilesizeY,int newTilesizeY,
				std::string& oldTileset,std::string newTileset){

				for(Textbox textbox : textboxes){
					for(char character : textbox.displayText){
						std::cout << character << std::endl;
					}
				}

				textboxes[1] = boxTilesizeX;
				textboxes[2] = boxTilesizeY;
				textboxes[3] = boxMapsize;
				textboxes[4] = boxTexture;
				std::cout << std::stoi("16")<<std::endl;
				std::cout << std::stoi("32")<<std::endl;

				std::cout << "Passed" <<std::endl;
				*/
				CreateTilemap(workingFile, textboxes[0].displayText, level, mapsize, textboxes[3].displayTextInt(), tilesizeX, textboxes[1].displayTextInt(), tilesizeY, textboxes[2].displayTextInt(), tileset, textboxes[4].displayText);

				InitializeNumOfTextures(tileset, numOfTexturesX, numOfTexturesY, numOfTextures, tilesizeX, tilesizeY);
				std::cout << "InitalizedTextures" << std::endl;

				textures.resize(numOfTextures);

				CreateSpriteSheet(textures, tileset, numOfTexturesX, numOfTexturesY, tilesizeX, tilesizeY);
				std::cout << "CreateSpriteSheet" << std::endl;

				tiles.resize(mapsize * mapsize);
				SetTileSprites(tiles, level, mapsize, tilesizeX, tilesizeY, textures);
				std::cout << "SetTileSprites" << std::endl;

				previewTiles.resize(numOfTextures);
				createPreviewTiles(previewTiles, textures, tilesizeX, tilesizeY, numOfTextures);
				std::cout << "createPreviewTiles" << std::endl;



			}
			
			else {
				for (int i = 0; i < numOfTextures; i++) {
					if (localMousePosition.x > previewTiles[i].x and
						localMousePosition.x < previewTiles[i].x + tilesizeX and
						localMousePosition.y > previewTiles[i].y and
						localMousePosition.y < previewTiles[i].y + tilesizeY) {
						std::cout << "Mouse Pressed at: " << localMousePosition.x << ", " << localMousePosition.y << std::endl;
						std::cout << "Selected tile: " << selectedTileType << std::endl;

						selectedTileType = i;
						break;


					}
				}
				for (int i = 0; i < 5; i++) {
					if (localMousePosition.x > textboxes[i].x and
						localMousePosition.x < textboxes[i].x + textboxes[i].width and
						localMousePosition.y > textboxes[i].y and
						localMousePosition.y < textboxes[i].y + textboxes[i].height) {
						std::cout << "Using Textbox: " << i << std::endl;

						selectedTextBox = i;
						break;


					}
				}




			}
		}


		window.clear();

		window.draw(menuBG);
		window.draw(menuTitle);
		window.draw(saveToJsonButton);
		window.draw(saveText);
		window.draw(loadFromJsonButton);
		window.draw(loadText);
		window.draw(createButton);
		window.draw(createText);
		//std::cout << "Tiles size: " << tiles.size() << std::endl;
		for (int x = 0; x < mapsize; x++) {
			for (int y = 0; y < mapsize; y++) {
				window.draw(tiles[y + x * mapsize].sprite);
			}
		}

		for (int i = 0; i < numOfTextures; i++){
			
			window.draw(previewTiles[i].sprite);
		}

		for (Textbox textbox : textboxes) {
			textbox.draw(window);
		}
		window.display();
	}

	return 0;
}

