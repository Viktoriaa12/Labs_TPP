
#include <SFML/Graphics.hpp>
#include <optional>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

constexpr unsigned int WINDOW_WIDTH = 1100;
constexpr unsigned int WINDOW_HEIGHT = 700;
constexpr float GRAVITY = 9.81f;

struct Projectile
{
    float speed = 25.0f;
    float angle = 45.0f;
    float time = 0.0f;
    float flightTime = 0.0f;
    float range = 0.0f;
    float maxHeight = 0.0f;
    bool launched = false;
    bool paused = false;
};

float toRadians(float degrees)
{
    return degrees * 3.14159265f / 180.0f;
}

float calculateFlightTime(float speed, float angle)
{
    return 2.0f * speed * std::sin(toRadians(angle)) / GRAVITY;
}

float calculateRange(float speed, float angle)
{
    return speed * speed * std::sin(2.0f * toRadians(angle)) / GRAVITY;
}

float calculateMaxHeight(float speed, float angle)
{
    float verticalSpeed = speed * std::sin(toRadians(angle));
    return verticalSpeed * verticalSpeed / (2.0f * GRAVITY);
}

sf::Text makeText(const sf::Font& font, const std::string& value,
    unsigned int size, sf::Vector2f position,
    sf::Color color = sf::Color::White)
{
    sf::Text text(font, value, size);
    text.setPosition(position);
    text.setFillColor(color);
    return text;
}

void drawLine(sf::RenderWindow& window, sf::Vector2f start,
    sf::Vector2f end, sf::Color color, float thickness = 2.0f)
{
    sf::Vector2f difference = end - start;
    float length = std::sqrt(
        difference.x * difference.x + difference.y * difference.y
    );

    if (length <= 0.0f)
        return;

    sf::RectangleShape line({ length, thickness });
    line.setPosition(start);
    line.setRotation(sf::degrees(
        std::atan2(difference.y, difference.x) * 180.0f / 3.14159265f
    ));
    line.setFillColor(color);
    window.draw(line);
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }),
        "Projectile Motion - Variant 7"
    );

    window.setFramerateLimit(60);

    sf::Font font;
    bool hasFont = font.openFromFile("C:/Windows/Fonts/arial.ttf");

    Projectile projectile;

    auto updatePhysics = [&]()
        {
            projectile.flightTime = calculateFlightTime(
                projectile.speed, projectile.angle
            );
            projectile.range = calculateRange(
                projectile.speed, projectile.angle
            );
            projectile.maxHeight = calculateMaxHeight(
                projectile.speed, projectile.angle
            );
        };

    auto resetFlight = [&]()
        {
            projectile.time = 0.0f;
            projectile.launched = false;
            projectile.paused = false;
        };

    updatePhysics();

    sf::Clock clock;

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                switch (key->code)
                {
                case sf::Keyboard::Key::Up:
                    projectile.speed = std::min(100.0f, projectile.speed + 1.0f);
                    updatePhysics();
                    resetFlight();
                    break;

                case sf::Keyboard::Key::Down:
                    projectile.speed = std::max(1.0f, projectile.speed - 1.0f);
                    updatePhysics();
                    resetFlight();
                    break;

                case sf::Keyboard::Key::Right:
                    projectile.angle = std::min(89.0f, projectile.angle + 1.0f);
                    updatePhysics();
                    resetFlight();
                    break;

                case sf::Keyboard::Key::Left:
                    projectile.angle = std::max(1.0f, projectile.angle - 1.0f);
                    updatePhysics();
                    resetFlight();
                    break;

                case sf::Keyboard::Key::Space:
                    if (!projectile.launched ||
                        projectile.time >= projectile.flightTime)
                    {
                        projectile.time = 0.0f;
                        projectile.launched = true;
                        projectile.paused = false;
                    }
                    else
                    {
                        projectile.paused = !projectile.paused;
                    }
                    break;

                case sf::Keyboard::Key::R:
                    resetFlight();
                    break;

                default:
                    break;
                }
            }
        }

        if (projectile.launched && !projectile.paused)
        {
            projectile.time += deltaTime;

            if (projectile.time >= projectile.flightTime)
            {
                projectile.time = projectile.flightTime;
                projectile.launched = false;
            }
        }

        window.clear(sf::Color(18, 24, 38));

        const float groundY = 540.0f;
        const float launchX = 75.0f;
        const float panelX = 750.0f;

        float scale = std::min(
            8.0f,
            std::min(
                620.0f / std::max(projectile.range, 1.0f),
                370.0f / std::max(projectile.maxHeight, 1.0f)
            )
        );

        auto toScreen = [&](float t)
            {
                float radians = toRadians(projectile.angle);

                float x = projectile.speed * std::cos(radians) * t;
                float y = projectile.speed * std::sin(radians) * t
                    - 0.5f * GRAVITY * t * t;

                return sf::Vector2f(
                    launchX + x * scale,
                    groundY - y * scale
                );
            };

        for (int x = 0; x <= 700; x += 50)
        {
            drawLine(window,
                { static_cast<float>(x), 100.0f },
                { static_cast<float>(x), groundY },
                sf::Color(39, 48, 65), 1.0f);
        }

        for (int y = 100; y <= static_cast<int>(groundY); y += 50)
        {
            drawLine(window,
                { 0.0f, static_cast<float>(y) },
                { 730.0f, static_cast<float>(y) },
                sf::Color(39, 48, 65), 1.0f);
        }

        drawLine(window, { 0.0f, groundY }, { 730.0f, groundY },
            sf::Color(100, 190, 130), 3.0f);

        float predictedTime = projectile.flightTime;

        for (int i = 0; i <= 100; ++i)
        {
            float t = predictedTime * i / 100.0f;
            sf::CircleShape dot(2.0f);
            dot.setOrigin({ 2.0f, 2.0f });
            dot.setPosition(toScreen(t));
            dot.setFillColor(sf::Color(70, 120, 180, 150));
            window.draw(dot);
        }

        if (projectile.launched || projectile.time > 0.0f)
        {
            int count = std::max(
                1,
                static_cast<int>(
                    100.0f * projectile.time /
                    std::max(projectile.flightTime, 0.001f)
                    )
            );

            for (int i = 0; i <= count; ++i)
            {
                float t = projectile.time * i / count;
                sf::CircleShape dot(3.0f);
                dot.setOrigin({ 3.0f, 3.0f });
                dot.setPosition(toScreen(t));
                dot.setFillColor(sf::Color(255, 180, 65));
                window.draw(dot);
            }
        }

        sf::Vector2f startPosition(launchX, groundY);
        float radians = toRadians(projectile.angle);
        sf::Vector2f projectilePosition = startPosition;

        if (projectile.launched || projectile.time > 0.0f)
            projectilePosition = toScreen(projectile.time);

        sf::CircleShape launcher(9.0f);
        launcher.setOrigin({ 9.0f, 9.0f });
        launcher.setPosition(startPosition);
        launcher.setFillColor(sf::Color(110, 180, 255));
        window.draw(launcher);

        sf::CircleShape ball(8.0f);
        ball.setOrigin({ 8.0f, 8.0f });
        ball.setPosition(projectilePosition);
        ball.setFillColor(sf::Color(255, 180, 65));
        window.draw(ball);

        drawLine(window, startPosition,
            {
                launchX + 55.0f * std::cos(radians),
                groundY - 55.0f * std::sin(radians)
            },
            sf::Color(255, 220, 120), 3.0f);

        sf::RectangleShape panel({ 330.0f, 650.0f });
        panel.setPosition({ panelX, 25.0f });
        panel.setFillColor(sf::Color(29, 37, 55));
        window.draw(panel);

        if (hasFont)
        {
            window.draw(makeText(font, "PROJECTILE MOTION", 23,
                { panelX + 20.0f, 45.0f }, sf::Color(120, 190, 255)));

            window.draw(makeText(font, "VARIANT 7", 15,
                { panelX + 20.0f, 80.0f }, sf::Color(160, 175, 195)));

            window.draw(makeText(font, "INITIAL PARAMETERS", 17,
                { panelX + 20.0f, 125.0f }, sf::Color(255, 210, 120)));

            window.draw(makeText(font,
                "Speed: " + std::to_string(static_cast<int>(projectile.speed))
                + " m/s",
                19, { panelX + 20.0f, 160.0f }));

            window.draw(makeText(font,
                "Angle: " + std::to_string(static_cast<int>(projectile.angle))
                + " deg",
                19, { panelX + 20.0f, 195.0f }));

            window.draw(makeText(font, "CALCULATED RESULTS", 17,
                { panelX + 20.0f, 245.0f }, sf::Color(255, 210, 120)));

            window.draw(makeText(font,
                "Flight time: " + std::to_string(projectile.flightTime)
                .substr(0, 5) + " s",
                17, { panelX + 20.0f, 280.0f }));

            window.draw(makeText(font,
                "Range: " + std::to_string(projectile.range)
                .substr(0, 5) + " m",
                17, { panelX + 20.0f, 315.0f }));

            window.draw(makeText(font,
                "Max height: " + std::to_string(projectile.maxHeight)
                .substr(0, 5) + " m",
                17, { panelX + 20.0f, 350.0f }));

            window.draw(makeText(font, "FLIGHT STATUS", 17,
                { panelX + 20.0f, 405.0f }, sf::Color(255, 210, 120)));

            std::string status = "Ready";

            if (projectile.launched && projectile.paused)
                status = "Paused";
            else if (projectile.launched)
                status = "In flight";
            else if (projectile.time >= projectile.flightTime)
                status = "Landed";

            window.draw(makeText(font, status, 19,
                { panelX + 20.0f, 440.0f }, sf::Color(130, 230, 160)));

            window.draw(makeText(font,
                "Elapsed: " + std::to_string(projectile.time)
                .substr(0, 5) + " s",
                17, { panelX + 20.0f, 475.0f }));

            window.draw(makeText(font, "CONTROLS", 17,
                { panelX + 20.0f, 520.0f }, sf::Color(255, 210, 120)));

            window.draw(makeText(font, "Up / Down: change speed", 14,
                { panelX + 20.0f, 552.0f }));

            window.draw(makeText(font, "Left / Right: change angle", 14,
                { panelX + 20.0f, 577.0f }));

            window.draw(makeText(font, "Space: launch / pause", 14,
                { panelX + 20.0f, 602.0f }));

            window.draw(makeText(font, "R: reset flight", 14,
                { panelX + 20.0f, 627.0f }));
        }

        window.display();
    }

    return 0;
}

