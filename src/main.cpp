#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <random>
#include <string>

//Random number generator
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<int> uni(128,1024);

int m_frame = 0;
int red_ghoul_attack_frame = 0;
int attack_frame = 0;

int statusx = 34;
int statusy = 23;

int health = 100;
int healthx = 32;
int healthy = 30;

int mana = 100;
int manax = 34;
int manay = 21;

int gold = 0;

int x = 0;
int y = 0;
float height = 0;
float width = 0;
float inity = 0;

// Define velocity, acceleration and gravity. I hate physics.

float velocityY = 0;
float accelerationY = 0;
float gravity = 0.85;

// A small boolean to keep track of whether our player is already jumping.
bool isJumping = false;
bool isPlaying = true;
bool isPause = false;
bool isAttacking1 = false;
bool isAttacking2 = false;
bool isAttacking3 = false;
bool isAttacking4 = false;

//Keeping track of whether or not the player character is performing an action

bool playerBusy = false;

// A function that makes sure our player moves nicely through space-time
void updateMovement() {
	if (y < inity)
		velocityY += gravity;
	else if (y > inity)
		y = inity;
	velocityY += accelerationY;
	y += velocityY;
}

int NumDigits(int x)  
{  
    x = abs(x);  
    return (x < 10 ? 1 :   
        (x < 100 ? 2 :   
        (x < 1000 ? 3 :   
        (x < 10000 ? 4 :   
        (x < 100000 ? 5 :   
        (x < 1000000 ? 6 :   
        (x < 10000000 ? 7 :  
        (x < 100000000 ? 8 :  
        (x < 1000000000 ? 9 :  
        10)))))))));  
}  

int main()
{
	width = 1920;
	height = 1080;
	
	// Create the main window and limit its framerate
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML window", sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	window.sf::Window::requestFocus();
	window.setVerticalSyncEnabled(true);

	float bg_ratio = height / 1024;
	
	// Initialize the background
	sf::Texture t_background;
	if (!t_background.loadFromFile("assets/background.jpg"))
		return EXIT_FAILURE;
	sf::Sprite background(t_background);
	background.setScale(bg_ratio, bg_ratio);
	
	sf::Font font;
	if (!font.loadFromFile("assets/font.ttf"))
		return EXIT_FAILURE;
	
	sf::Texture t_status;
	if (!t_status.loadFromFile("assets/status_bar.png"))
		return EXIT_FAILURE;
	sf::Sprite status_bar(t_status);

	sf::Texture t_health;
	if(!t_health.loadFromFile("assets/health_bar.png"))
		return EXIT_FAILURE;
	sf::Sprite health_bar(t_health);

	sf::Texture t_mana;
	if(!t_mana.loadFromFile("assets/mana_bar.png"))
		return EXIT_FAILURE;
	sf::Sprite mana_bar(t_mana);

	// Initialize the player character
	sf::Texture t_player;
	if (!t_player.loadFromFile("assets/player.png"))
		return EXIT_FAILURE;
	sf::Sprite player(t_player);

	//Initialize the bullet
	sf::Texture t_bullet;
	if (!t_bullet.loadFromFile("assets/bullet.png"))
		return EXIT_FAILURE;
	sf::Sprite bullet(t_bullet);
	
	//Initialize the endscreen graphic
	sf::Texture t_endscreen;
	if (!t_endscreen.loadFromFile("assets/endscreen.png"))
		return EXIT_FAILURE;
	sf::Sprite endscreen(t_endscreen);
	
	sf::Texture t_menu;
	if (!t_menu.loadFromFile("assets/menu.png"))
		return EXIT_FAILURE;
	sf::Sprite menu(t_menu);


	//Initialize the red ghoul
	sf::Texture t_red_ghoul;
	if (!t_red_ghoul.loadFromFile("assets/red_ghoul.png"))
		return EXIT_FAILURE;
	sf::Sprite red_ghoul1(t_red_ghoul);
	sf::Sprite red_ghoul2(t_red_ghoul);
	sf::Sprite red_ghoul3(t_red_ghoul);
	sf::Sprite red_ghoul4(t_red_ghoul);

	// Add a clock
	sf::Clock clock;
	sf::Clock attackClock;
	sf::Clock attackTimer;
	sf::Clock mana_clock;
	sf::Clock endClock;
	sf::Clock rg_attackClock;

	sf::Vector2i mouse;

	status_bar.setPosition(statusx, statusy);
	status_bar.setScale(0.862*2.5, 0.862*2.5);
	health_bar.setPosition(healthx+2, healthy-9);
	health_bar.setScale(0.862*2.5, 0.862*2.5);
	mana_bar.setPosition(manax, manay);
	mana_bar.setScale(0.862*2.5, 0.862*2.5);
	
	sf::Text health_val;
	std::string health_str;
	health_val.setFont(font);
	health_val.setCharacterSize(10);
	health_val.setFillColor(sf::Color::Black);
	health_val.setPosition(healthx+2 + 108*bg_ratio, healthy-5 + 62*bg_ratio);

	sf::Text mana_val;
	std::string mana_str;
	mana_val.setFont(font);
	mana_val.setCharacterSize(10);
	mana_val.setFillColor(sf::Color::White);
	mana_val.setPosition(healthx+3 + 108*bg_ratio, healthy-9 + 75*bg_ratio);
	
	sf::Text gold_val;
	std::string gold_str;
	gold_val.setFont(font);
	gold_val.setCharacterSize(16);
	gold_val.setFillColor(sf::Color::Yellow);
	
	float player_scale = 3.5 * bg_ratio;
	
	// Set player coordinates
	inity = height - (height/7) - (50*player_scale);
	y = inity;
	x = 128+128;
	
	float leftedge = 128 + 64;
	float rightedge = width - 128 - 64;
	
	player.setPosition(x, y);
	player.setOrigin(104/2, 95/2); // Set the origin to roughly the center of the sprite, for collision detection purposes
	player.setScale(player_scale, player_scale);  // Resize the player - a bit too small for comfort.
	player.setTextureRect(sf::IntRect(1, 1, 95, 104)); // Switch to the first frame of the player spritesheet
	
	float rg_scale = 4.5*bg_ratio;
	float rg1_x = width + uni(rng);
	float rg_y = height - (height/7) - (16*rg_scale);
	float rg2_x = width + uni(rng);
	float rg3_x = width + uni(rng);
	float rg4_x = width + uni(rng);
	
	red_ghoul1.setPosition(rg1_x, rg_y);
	red_ghoul1.setOrigin(63/2, 55/2);
	red_ghoul1.setScale(rg_scale, rg_scale);
	red_ghoul1.setTextureRect(sf::IntRect(63*0, 0, 63, 55));

	red_ghoul2.setPosition(rg2_x, rg_y);
	red_ghoul2.setOrigin(63/2, 55/2);
	red_ghoul2.setScale(rg_scale, rg_scale);
	red_ghoul2.setTextureRect(sf::IntRect(63*0, 0, 63, 55));

	red_ghoul3.setPosition(rg3_x, rg_y);
	red_ghoul3.setOrigin(63/2, 55/2);
	red_ghoul3.setScale(rg_scale, rg_scale);
	red_ghoul3.setTextureRect(sf::IntRect(63*0, 0, 63, 55));

	red_ghoul4.setPosition(rg4_x, rg_y);
	red_ghoul4.setOrigin(63/2, 55/2);
	red_ghoul4.setScale(rg_scale, rg_scale);
	red_ghoul4.setTextureRect(sf::IntRect(63*0, 0, 63, 55));
	
	int endscreenx = (width/2) - 269*1.5;
	int endscreeny = -800;
	endscreen.setPosition(endscreenx, endscreeny);
	endscreen.setTextureRect(sf::IntRect(0, 0, 539, 523));
	endscreen.setScale(1.5, 1.5);
	
	int menux = (width/2) - 269*1.5;
	int menuy = -800;
	menu.setPosition(menux, menuy);
	menu.setTextureRect(sf::IntRect(0, 0, 539, 523));
	menu.setScale(1.5, 1.5);

	// Start the game loop
	while (window.isOpen())
	{
		// Events - like jumping and closing the window and shooting bullets.
		// First of all, let's make sure we aren't flying into outer space
		// if our player holds the Space key for too long.

		window.setKeyRepeatEnabled(false);
		mouse = sf::Mouse::getPosition();

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				// Let's make sure the window actually closes when we want it to.
				case sf::Event::Closed:
					window.close();
					break;

				// Let's teach our pirate how to jump.
				// And by that, I mean let's launch him into the air at a speed of 10 pixels per frame.
				case sf::Event::KeyPressed:
					if (isPlaying) {
						if (event.key.code == sf::Keyboard::Up) {
							if (isJumping == false) {
								velocityY = -25;
								isJumping = true;
							}
						}
						if (event.key.code == sf::Keyboard::Space) {
							if ((playerBusy == false) and (mana >= 15)) {
								playerBusy = true;
								player.setTextureRect(sf::IntRect(189, 1, 94, 103));
								mana -= 15;
							}
						}
					}
						if (event.key.code == sf::Keyboard::Escape) {
							if (isPause == false)	
								isPause = true;
							else {
								isPause = false;
							}
						}
						if (event.key.code == sf::Keyboard::F4) {
							health = 0;
						}
					break;
				default:
					break;
			}
		}
if (isPlaying) {
		//Let the player skedaddle across the screen, but not too far - inside the boundaries we defined earlier
			if ((x >= leftedge) && (x <= rightedge)) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				{
					if (isJumping) {
						x = x + 9;
					}
					else {
						x = x + 6;
					}
					player.setTextureRect(sf::IntRect(95, 1, 95, 104));
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				{
					if (isJumping) {
						x = x - 9;
					}
					else {
						x = x - 6;
					}
					player.setTextureRect(sf::IntRect(1, 1, 95, 104));
				}

				//Sometimes the player ends up magically outside the boundaries. We're slapping him back into place.
				if (x < leftedge)
					x = leftedge;

				if (x > rightedge)
					x = rightedge;
			}
		
			if (playerBusy) {
					if (attackClock.getElapsedTime().asSeconds() > 0.017f) {
						attack_frame++;
						switch (attack_frame) {
							case 1:
								player.setTextureRect(sf::IntRect(189, 1, 94, 103));
								attackClock.restart();
								break;
							case 2:
								player.setTextureRect(sf::IntRect(393, 1, 105, 105));
								attackClock.restart();
								break;
							case 3:
								player.setTextureRect(sf::IntRect(-1, 105, 107, 103));
								attackClock.restart();
								break;
							case 4:
								player.setTextureRect(sf::IntRect(283, 1, 111, 105));
								attackClock.restart();
								break;
							case 5:
								player.setTextureRect(sf::IntRect(105, 105, 122, 102));
								attackClock.restart();
								break;
							case 6:
								player.setTextureRect(sf::IntRect(227, 105, 117, 102));
								attackClock.restart();
								break;
							case 7:
								player.setTextureRect(sf::IntRect(345, 105, 114, 102));
								attackClock.restart();
								break;
							case 8:
								attack_frame = 1;
								player.setTextureRect(sf::IntRect(189, 1, 94, 103));
								playerBusy = false;
								attackClock.restart();
								if (((rg1_x - x > 0) and (rg1_x - x < 300)) and std::abs(y-rg_y)<200) {
									rg1_x = width + uni(rng);
									gold = gold + 5;
								}
								if (((rg2_x - x > 0) and (rg2_x - x < 300)) and std::abs(y-rg_y)<200) {
									rg2_x = width + uni(rng);
									gold = gold + 5;

								}
								if (((rg3_x - x > 0) and (rg3_x - x < 300)) and std::abs(y-rg_y)<200) {
									rg3_x = width + uni(rng);
									gold = gold + 5;

								}
								if (((rg4_x - x > 0) and (rg4_x - x < 300)) and std::abs(y-rg_y)<200) {
									rg4_x = width + uni(rng);
									gold = gold + 5;

								}
							default:
								attack_frame = 1;
								attackClock.restart();
						}
					}
			}
		// Update the player's position
		updateMovement();
		
		if (y > inity)
			y = inity;

		if (y == (int)inity)
			isJumping = false;

		player.setPosition(x,y);


		// Animate the red ghoul
		if (clock.getElapsedTime().asSeconds() > 0.20f) {
			if (isAttacking1 == false) {
				rg_y = height - (height/7) - (16*rg_scale);
				if (m_frame >=3) {
					m_frame = 0;
				}
				if (rg1_x - x > 128 -16) {
					rg1_x -= 24;
				}
				else if (rg1_x - x < -(128 - 16)) {
					rg1_x += 24;
				}
				switch (m_frame) {
					case 0:
						red_ghoul1.setTextureRect(sf::IntRect(140, 70, 140 + 63, 70+55));
						break;
					case 1:
						red_ghoul1.setTextureRect(sf::IntRect(0, 140, 63, 140 + 55));
						break;
					case 2:
						red_ghoul1.setTextureRect(sf::IntRect(63, 140, 63*2, 140 + 55));
						break;
				}
				m_frame++;
			}
			clock.restart();
		}
		if (rg_attackClock.getElapsedTime().asSeconds() > 0.10f) {
			if ((std::abs(rg1_x - x) <= (128 + 16)) and std::abs(y-rg_y)<200) {
				isAttacking1 = true;
				if (red_ghoul_attack_frame >= 10) {
					red_ghoul_attack_frame = 0;
					rg_y = height - (height/7) - (16*rg_scale);
				}
				switch (red_ghoul_attack_frame) {
					case 0:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 1:
						red_ghoul1.setTextureRect(sf::IntRect(70, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 2:
						red_ghoul1.setTextureRect(sf::IntRect(140, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 3:
						red_ghoul1.setTextureRect(sf::IntRect(0, 70, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 4:
						red_ghoul1.setTextureRect(sf::IntRect(70, 70, 70, 70));
						health -= 20;
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 5:
						red_ghoul1.setTextureRect(sf::IntRect(70, 70, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 6:
						red_ghoul1.setTextureRect(sf::IntRect(140, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 7:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 8:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 9:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
						
				}
				red_ghoul_attack_frame++;
			}
			else {
				if (red_ghoul_attack_frame < 10 and red_ghoul_attack_frame > 0) {
					switch (red_ghoul_attack_frame) {
						case 0:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 1:
						red_ghoul1.setTextureRect(sf::IntRect(70, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 2:
						red_ghoul1.setTextureRect(sf::IntRect(140, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 3:
						red_ghoul1.setTextureRect(sf::IntRect(0, 70, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 4:
						red_ghoul1.setTextureRect(sf::IntRect(70, 70, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 5:
						red_ghoul1.setTextureRect(sf::IntRect(70, 70, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 6:
						red_ghoul1.setTextureRect(sf::IntRect(140, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 7:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 8:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
					case 9:
						red_ghoul1.setTextureRect(sf::IntRect(0, 0, 70, 70));
						rg_y = height - (height/7) - (16*rg_scale) - 30;
						break;
						
					}
					red_ghoul_attack_frame++;
					if (red_ghoul_attack_frame >= 10) {
						red_ghoul_attack_frame = 0;
						rg_y = height - (height/7) - (16*rg_scale);
						red_ghoul1.setTextureRect(sf::IntRect(140, 70, 140 + 63, 70+55));
					}
				}
				else {
					isAttacking1 = false;
					if (red_ghoul_attack_frame = 0) {
						rg_y = height - (height/7) - (16*rg_scale);
						red_ghoul1.setTextureRect(sf::IntRect(140, 70, 140 + 63, 70+55));
					}
					if (red_ghoul_attack_frame >= 10) {
						red_ghoul_attack_frame = 0;
						rg_y = height - (height/7) - (16*rg_scale);
						red_ghoul1.setTextureRect(sf::IntRect(140, 70, 140 + 63, 70+55));
					}
					
				}
			}
			rg_attackClock.restart();
		}
}		
		if ((mana > 100) or (mana < 0)) {
			mana = 0;
		}
		
		if (mana_clock.getElapsedTime().asSeconds() > 0.20f && isPause == false && health != 0) {
			if (mana < 100) {
				mana += 1;
				mana_clock.restart();
			}
		}
		
		if (health == 0) {
			isPlaying = false;
		}
		
		if (isPlaying == false && health == 0) {
			player.setColor(sf::Color(255, 255, 255, 150));
			if (endscreeny != -20) {
				if (endClock.getElapsedTime().asSeconds() > 0.020f) {
					endscreeny = endscreeny + 20;
					endscreen.setPosition(endscreenx, endscreeny);
					endClock.restart();
				}
			}
			if (endscreeny == -20) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
					return EXIT_SUCCESS;
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (((mouse.x > endscreenx + 156) && (mouse.x < endscreenx + 535)) && ((mouse.y > endscreeny - 20 + 275) && (mouse.y < endscreeny - 20 + 335))) {
						rg1_x = width + 256;
						rg_y = height - (height/7) - (16*rg_scale);
						rg2_x = width + 256 + 256;
						rg3_x = width + 256 + 512;
						rg4_x = width + 256 + 512 + 256;
						y = inity;
						x = 128+128;
						health = 100;
						mana = 100;
						gold = 0;
						player.setPosition(x, y);
						red_ghoul1.setPosition(rg1_x, rg_y);
						red_ghoul2.setPosition(rg2_x, rg_y);
						red_ghoul3.setPosition(rg3_x, rg_y);
						red_ghoul4.setPosition(rg4_x, rg_y);
						player.setColor(sf::Color(255, 255, 255, 255));
						isPlaying = true;
					}
					if (((mouse.x > endscreenx + 200) && (mouse.x < endscreenx + 570)) && ((mouse.y > endscreeny - 20 + 666) && (mouse.y < endscreeny - 20 + 735))) {
						return EXIT_SUCCESS;
					}
				}
			}
		}
		
		if (isPause == true && health != 0) {
			isPlaying = false;
			if (menuy != -20) {
				if (endClock.getElapsedTime().asSeconds() > 0.020f) {
					menuy = menuy + 20;
					menu.setPosition(menux, menuy);
					endClock.restart();
				}
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					if (((mouse.x > menux + 156) && (mouse.x < menux + 535)) && ((mouse.y > menuy - 20 + 275) && (mouse.y < menuy - 20 + 335))) {
						isPause = false;
					}
					if (((mouse.x > menux + 200) && (mouse.x < menux + 570)) && ((mouse.y > menuy - 20 + 666) && (mouse.y < menuy - 20 + 735))) {
						return EXIT_SUCCESS;
					}
				}
		}
		
		if (isPause == false && health != 0) {
			if (menuy != -800) {
				isPlaying = true;
				if (endClock.getElapsedTime().asSeconds() > 0.001f) {
					menuy = menuy - 20;
					menu.setPosition(menux, menuy);
					endClock.restart();
				}
			}
			if (endscreeny != -800) {
				if (endClock.getElapsedTime().asSeconds() > 0.001f) {
					endscreeny = endscreeny - 20;
					endscreen.setPosition(endscreenx, endscreeny);
					endClock.restart();
				}
			}
		}
		health_bar.setTextureRect(sf::IntRect(0, 0, health/0.862, 58*0.862));
		mana_bar.setTextureRect(sf::IntRect(0, 0, mana/0.862, 58*0.862));
		health_str = std::to_string(health);
		health_val.setString(health_str);
		
		mana_str = std::to_string(mana);
		mana_val.setString(mana_str);
		
		gold_str = std::to_string(gold);
		gold_val.setString(gold_str);
		
		int goldpos = NumDigits(gold);
		gold_val.setPosition(width - (16 + 13*goldpos), 16);
		
		red_ghoul1.setPosition(rg1_x, rg_y);
		red_ghoul2.setPosition(rg2_x, rg_y);
		red_ghoul3.setPosition(rg3_x, rg_y);
		red_ghoul4.setPosition(rg4_x, rg_y);

		// Clear the screen, redraw the sprites and render them;
		window.draw(background);
		window.draw(player);
		window.draw(red_ghoul1);
		window.draw(red_ghoul2);
		window.draw(red_ghoul3);
		window.draw(red_ghoul4);
		window.draw(status_bar);
		window.draw(health_bar);
		window.draw(mana_bar);
		window.draw(health_val);
		window.draw(mana_val);
		window.draw(gold_val);
		window.draw(endscreen);
		window.draw(menu);
		window.display();
	}
	return EXIT_SUCCESS;
}
