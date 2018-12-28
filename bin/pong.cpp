#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

//CSCI 437 HW 1 Pong
//Roshan Manjaly
//Professor Peers
//All Core Requirements Met
//Added Functionality:
//         Textured Background
//         Background Music
//         Numerous Sound Effects (Ball hitting the wall, Ball hitting the paddle, scoring a point, and an applause for if the player wins)
//         Pause Screen (enabled by pressing space while playing
//         The Ability to reset the game at any point while playing

int main(int argc, char** argv)
{
  
    //Main Variables
    const int appWidth = 800;
    const int appHeight = 600;
    sf::Vector2f padSize((appWidth/40.f), (appHeight/6.f));
    int ballSize = appWidth/80.f;
    int padWallDistance = 20;
    
    //Addition of Music
    sf::Music music;
    if (!music.openFromFile("../Assets/PongWorld.wav")) 
        return -1;
    music.setVolume(40);
    music.play();
    music.setLoop(true);
    
    
    //Addition of Sounds
    sf::SoundBuffer buffer1; // Ball hitting the walls
    sf::SoundBuffer buffer2; // Ball hitting paddle
    sf::SoundBuffer buffer3; // Point Loss
    sf::SoundBuffer buffer4; // Applause
    
    
    //Sound Buffers
    if (!buffer1.loadFromFile("../Assets/b_wall.wav"))
        return -1;    
    if (!buffer2.loadFromFile("../Assets/b_paddle.wav"))
        return -1;
    if (!buffer3.loadFromFile("../Assets/loss_of_point.wav"))
        return -1;
    if (!buffer4.loadFromFile("../Assets/applause.wav"))
        return -1;
    
    sf::Sound ballWall;
    sf::Sound ballPaddle;
    sf::Sound pointMade;
    sf::Sound applause;
    
    ballWall.setBuffer(buffer1);
    ballPaddle.setBuffer(buffer2);
    pointMade.setBuffer(buffer3);
    applause.setBuffer(buffer4);
    
    
    ballWall.setVolume(100);    
    ballPaddle.setVolume(100);    
    pointMade.setVolume(100);
    applause.setVolume(100);
    
    //Addition of Font
    sf::Font font;
    if(!font.loadFromFile("../Assets/BebasNeue.otf")) {}
    
    //Addition of Title Sequence (Video would be cool)
    
    
    
    //Anti Aliasing Support - should make the objects visually smoother if possible
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    
    sf::RenderWindow App(sf::VideoMode(appWidth,appHeight,32), "Pong - SFML", sf::Style::Titlebar | sf::Style::Close); //Resizing turned off
    App.setVerticalSyncEnabled(true);
    
    //Adding a background
    sf::Texture texture;
    if (!texture.loadFromFile("../Assets/background.png"))
        return -1;
    sf::Sprite background(texture);
    
    //Characteristics for the Ball
    sf::CircleShape ball;
    ball.setRadius(ballSize);
    ball.setFillColor(sf::Color(0,0,0,0));
    ball.setOrigin(ballSize/2, ballSize/2); //<--note this is change the center point of the object
    
    
    //Characteristics for the Left Pad - most likely the computer
    sf::RectangleShape leftPad;
    leftPad.setSize(padSize);
    leftPad.setFillColor(sf::Color::White);
    leftPad.setOrigin(padSize/2.f);//<--note this is change the center point of the object
    
    // Characteristics for right pad - most likly Player 1
    sf::RectangleShape rightPad;
    rightPad.setSize(padSize);
    rightPad.setFillColor(sf::Color::White);
    rightPad.setOrigin(padSize/2.f);//<--note this is change the center point of the object
    
    //Test States
    ball.setPosition(appWidth/2, appHeight/2);
    leftPad.setPosition(padSize.x + padWallDistance, appHeight/2);
    rightPad.setPosition(appWidth - padSize.x - padWallDistance, appHeight/2);
    
    //Main Title Message (doesn't change)
    sf::Text message;
    message.setFont(font);
    message.setString("Welcome to Pong");
    message.setCharacterSize(100);
    message.setFillColor(sf::Color(249,81,58));
    message.setStyle(sf::Text::Bold | sf::Text::Underlined);
    
    //Formatting
    sf::FloatRect textRect = message.getLocalBounds();
    message.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
    message.setPosition(appWidth/2.0f, appHeight/8.0f);
    
    //Start and also Score message board
    sf::Text outputScore;
    outputScore.setFont(font);
    outputScore.setString("Press the Space Bar to Start Playing");
    outputScore.setCharacterSize(50);
    outputScore.setFillColor(sf::Color::White);
    outputScore.setStyle(sf::Text::Bold);
    
    //Formatting
    sf::FloatRect scoreRect = outputScore.getLocalBounds();
    outputScore.setOrigin(scoreRect.left + scoreRect.width/2.0f, scoreRect.top + scoreRect.height/2.0f);
    outputScore.setPosition(appWidth/2.0f, appHeight-scoreRect.height);
    
    //Rules and Congratulations
    sf::Text rulesAndCongrats;
    rulesAndCongrats.setFont(font);
    rulesAndCongrats.setString("  Rules:\n\tThe Ball will randomly choose a direction each round\n\tPress the Space Bar to Start and Pause\n\tPress 'R' to Reset\n\tPress 'Q' to Quit");
    rulesAndCongrats.setCharacterSize(35);
    rulesAndCongrats.setFillColor(sf::Color::White);
    rulesAndCongrats.setStyle(sf::Text::Bold);
    
    //Formatting
    sf::FloatRect rAndCRect = rulesAndCongrats.getLocalBounds();
    rulesAndCongrats.setOrigin(rAndCRect.left + rAndCRect.width/2.0f, rAndCRect.top + rAndCRect.height/2.0f);
    rulesAndCongrats.setPosition(appWidth/2.0f, appHeight/2.0f);
    
    //Randomization Factors
    int factor;
    int factor_two;
    if((rand() % 2 - (1)) < 0)
        factor = -1;
    else
        factor = 1;
    
    if((rand() % 2 - (1)) < 0)
        factor_two = -1;
    else
        factor_two = 1;
    
    
    //Game properties
    int aiPoints = 0;
    int playerPoints = 0;
    int winningPoints = 11;
    bool gameStatus = false;
    bool pause = false;
    bool gameOver = false;
    float moveSpeed = 250.f;
    float moveSpeedcopy = moveSpeed;
    float baseBallSpeed = 5.f;
    float variationFactor = 20; // angle variaton to be added
    sf::Clock clock;
    float internalClock = clock.getElapsedTime().asSeconds();
    sf::Vector2f ballSpeed(baseBallSpeed * factor, baseBallSpeed * abs(atan(rand() * variationFactor + 1)*factor_two));
    
    //pause after scoring
    sf::Time wait = sf::milliseconds(500);
    sf::Time audioWait = sf::milliseconds(125);
    
    //variable used for padding
    int padTop;
    int padBottom;
    int padInside;
    int ballx;
    int ball_y_top;
    int ball_y_bottom;
    
    
    //Main Game Cyle
    while(App.isOpen())
    {
        // process events
        sf::Event Event;
        while(App.pollEvent(Event))
        {
            
            // Exit
            if((Event.type == sf::Event::Closed) || ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Q)))
                //Uses number Q key to quit the games
                App.close();
            
            // Reset
            if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::R) && gameStatus) 
                //Press R to Reset the Game
            {
                outputScore.setString("Press the Space Bar to Start Playing");
                rulesAndCongrats.setString("");
                message.setCharacterSize(100);
                message.setString("Welcome to Pong");
                ball.setPosition(appWidth/2, appHeight/2);
                leftPad.setPosition(padSize.x + padWallDistance, appHeight/2);
                rightPad.setPosition(appWidth - padSize.x - padWallDistance, appHeight/2);
                
                textRect = message.getLocalBounds();
                message.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
                message.setPosition(appWidth/2.0f, appHeight/8.0f);
                
                
                aiPoints = 0;
                playerPoints = 0;
                gameStatus = false;
                pause = false;
                gameOver = false;
            }
            
            // Used to Start and pause the Game
            if((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Space)) 
            {
                clock.restart();
                if(gameStatus == false)
                {
                    ball.setFillColor(sf::Color::White);
                    ball.setPosition(appWidth/2, appHeight/2);
                    leftPad.setPosition(padSize.x + padWallDistance, appHeight/2);
                    rightPad.setPosition(appWidth - padSize.x - padWallDistance, appHeight/2);
                    gameStatus = true;
                    rulesAndCongrats.setString("");
                }
                else if(gameOver)
                {
                }
                else
                {
                    if(pause == false)
                    {
                        outputScore.setString("Press 'Space Bar' to resume Game");
                        pause = true;
                    }
                    else
                        pause = false;
                }
            }
        }
        
        //During game functions
        if(gameStatus == true && pause == false)
        {
            internalClock = clock.restart().asSeconds();
            
            // Collision Aids - Bounding Boxes
            sf::FloatRect ballBoundingBox = ball.getGlobalBounds();
            sf::FloatRect leftPadBoundingBox = leftPad.getGlobalBounds();
            sf::FloatRect rightPadBoundingBox = rightPad.getGlobalBounds();
            
            
            // User Input
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && ((rightPad.getPosition().y - padSize.y/2)> 1))
            {
                rightPad.move(0, -(moveSpeed * internalClock));
            }
            
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && ((rightPad.getPosition().y + padSize.y/2) < appHeight - 1))
            {
                rightPad.move(0, (moveSpeed * internalClock));
            }
            
            //AI Movment
            if( ((leftPad.getPosition().y - padSize.y/2) > 5) || ((leftPad.getPosition().y + padSize.y/2) < appHeight - 5))
            {
                moveSpeedcopy = moveSpeed * internalClock;
                
                if (leftPad.getPosition().y > ball.getPosition().y)
                    moveSpeedcopy = -moveSpeedcopy;
                else if (leftPad.getPosition().y < ball.getPosition().y)
                    moveSpeedcopy = moveSpeedcopy;
                
                
                padTop = leftPad.getPosition().y - padSize.y/2;
                padBottom = leftPad.getPosition().y + padSize.y/2;
                
                leftPad.move(0,0 + moveSpeedcopy);
                
            }
            
            //Collision Testing walls --> Sets the scores
            ball.move(ballSpeed.x, ballSpeed.y);
            
            
            //Collisions agains walls
            if((ball.getPosition().x + ballSize > appWidth) || (ball.getPosition().x - ballSize < 0))
            {
                ballSpeed.x = -ballSpeed.x;
                
                if(ball.getPosition().x > appWidth/2)
                {
                    pointMade.play();
                    sf::sleep(audioWait);
                    aiPoints = aiPoints + 1;
                    sf::sleep(audioWait);
                    pointMade.play();
                }
                if(ball.getPosition().x < appWidth/2)
                {
                    pointMade.play();
                    sf::sleep(audioWait);
                    playerPoints = playerPoints + 1;
                    pointMade.play();
                    sf::sleep(audioWait);
                }
                
                ball.setPosition(appWidth/2, appHeight/2);
                
                
                //Randomization for each round
                if((rand() % 2 - (1)) < 0)
                    factor = -1;
                else
                    factor = 1;
                
                
                if((rand() % 2 - (1)) < 0)
                    factor_two = -1;
                else
                    factor_two = 1;
                
                ballSpeed.x = baseBallSpeed * factor;
                ballSpeed.y = baseBallSpeed * factor_two;
                
                //Pause after scoring
                sf::sleep(wait);
            }
            
            //Collision Testing ceilings and bottom
            if((ball.getPosition().y + ballSize > appHeight) || (ball.getPosition().y - ballSize < 0))
            {
                ballWall.play();
                ballSpeed.y = -(ballSpeed.y);
                ballSpeed.y = (ballSpeed.y + abs(sin(rand() % 90 + 1)));
                ballWall.play();
            }
            
            // Collisions Detection with left Pad
            if((ballBoundingBox.intersects(leftPadBoundingBox))) 
            {
                padTop = leftPad.getPosition().y - padSize.y/2;
                padBottom = leftPad.getPosition().y + padSize.y/2;
                padInside = leftPad.getPosition().x + padSize.x/2 + ballSize*2;
                ballx = ball.getPosition().x + ballSize;
                ball_y_top = ball.getPosition().y - ballSize;
                ball_y_bottom = ball.getPosition().y + ballSize;
                //left paddle
                
                if(ball.getPosition().x  <= padInside)
                {
                    //ball bounces back inward
                    ballPaddle.play();
                    ballSpeed.x = -ballSpeed.x;
                    ball.move(ballSize,0.f);
                    ballPaddle.play();
                }
                else if((ball_y_bottom >= padTop)  && (ballx != padInside) )
                {
                    //ball bounces back up
                    ballPaddle.play();
                    ballSpeed.y = -ballSpeed.y;
                    ball.move(0.f,-ballSize*1.f);
                    ballPaddle.play();
                }
                else if((ball_y_top <= padBottom)  && (ballx != padInside) )
                {
                    //ball bounces back down
                    ballPaddle.play();
                    ballSpeed.y = -ballSpeed.y;
                    ball.move(0.f,ballSize*1.f);
                    ballPaddle.play();
                }
                
            }
            
            // Collisions Detection with Right Pad 
            if((ballBoundingBox.intersects(rightPadBoundingBox)))
            {
                padTop = rightPad.getPosition().y - padSize.y/2;
                padBottom = rightPad.getPosition().y + padSize.y/2;
                padInside = rightPad.getPosition().x - padSize.x/2;
                ballx = ball.getPosition().x + ballSize;
                ball_y_top = ball.getPosition().y - ballSize;
                ball_y_bottom = ball.getPosition().y + ballSize;
                //right paddle
                
                if(ballx >= padInside)
                {
                    //ball bounces back inward
                    ballPaddle.play();
                    ballSpeed.x = -ballSpeed.x;
                    ball.move(-ballSize*1.f,0.f);
                    ballPaddle.play();
                }
                else if((ball_y_bottom >= padTop)  && (ballx != padInside) )
                {
                    //ball bounces back up
                    ballPaddle.play();
                    ballSpeed.y = -ballSpeed.y;
                    ball.move(0.f,-ballSize*1.f);
                    ballPaddle.play();
                    
                }
                else if((ball_y_top <= padBottom)  && (ballx != padInside) )
                {
                    //ball bounces back down
                    ballPaddle.play();
                    ballSpeed.y = -ballSpeed.y;
                    ball.move(0.f,ballSize*1.f);
                    ballPaddle.play();
                }
            }
            outputScore.setString("Ai Score: " + std::to_string(aiPoints) + " | Player Score: " + std::to_string(playerPoints));       
        }
        
        
        //Declaration of Winner
        if(aiPoints == winningPoints || playerPoints == winningPoints)
        {
            pause = true;
            gameOver=true;
            
            if (aiPoints == winningPoints)
            {
                message.setString("You Lost");
            }
            
            if (playerPoints == winningPoints)
            {
                message.setCharacterSize(75);
                message.setString("Congratulations!! You Won");
                applause.play();
            }
            
            rulesAndCongrats.setString("\tRestart: 'R'\n\tQuit: 'Escape' or 'Q'");
            
            textRect = message.getLocalBounds();
            message.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
            message.setPosition(appWidth/2.0f, appHeight/8.0f);
            
        }
        
        
        //Clock Update
        internalClock = clock.getElapsedTime().asSeconds();
        
        //Formatting adjustment to make sure text is perfectly centered
        scoreRect = outputScore.getLocalBounds();
        outputScore.setOrigin(scoreRect.left + scoreRect.width/2.0f, scoreRect.top + scoreRect.height/2.0f);
        outputScore.setPosition(appWidth/2.0f, appHeight-scoreRect.height);
        
        
        //Then clear and draw
        App.clear(sf::Color::Black);
        App.draw(background);
        App.draw(message);
        App.draw(outputScore);
        App.draw(rulesAndCongrats);
        App.draw(ball);
        App.draw(leftPad);
        App.draw(rightPad);
        App.display();
    }
    
    // Done.
    return 0;
}
