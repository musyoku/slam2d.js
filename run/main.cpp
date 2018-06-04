#include "include.h"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <vector>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1920, 800, "slam2d", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    bool show_demo_window = true;
    bool show_another_window = false;
    glm::vec4 bg_color = glm::vec4(61.0f / 255.0f, 57.0f / 255.0f, 90.0f / 255.0f, 1.00f);

    std::unique_ptr<slam::environment::Field> field = std::make_unique<slam::environment::Field>();
    std::unique_ptr<slam::lidar::Ovserver> observer = std::make_unique<slam::lidar::Ovserver>();

    float wall_thickness = 0.05;
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, -1);
        wall.emplace_back(-1, -1 + wall_thickness);
        wall.emplace_back(1, -1);
        wall.emplace_back(1, -1 + wall_thickness);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1 + wall_thickness, -1);
        wall.emplace_back(-1, -1);
        wall.emplace_back(-1 + wall_thickness, 1);
        wall.emplace_back(-1, 1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-1, 1 - wall_thickness);
        wall.emplace_back(-1, 1);
        wall.emplace_back(1, 1 - wall_thickness);
        wall.emplace_back(1, 1);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(1, -1);
        wall.emplace_back(1 - wall_thickness, -1);
        wall.emplace_back(1, 1);
        wall.emplace_back(1 - wall_thickness, 1);
        field->add_wall(wall);
    }

    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.2, -0.3);
        wall.emplace_back(-0.2, -0.3 + wall_thickness);
        wall.emplace_back(0.3, -0.3);
        wall.emplace_back(0.3, -0.3 + wall_thickness);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.4 + wall_thickness, -0.3);
        wall.emplace_back(-0.4, -0.3);
        wall.emplace_back(-0.3 + wall_thickness, 0.3);
        wall.emplace_back(-0.3, 0.3);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(-0.3, 0.3 - wall_thickness);
        wall.emplace_back(-0.3, 0.3);
        wall.emplace_back(0.4, 0.3 - wall_thickness);
        wall.emplace_back(0.4, 0.3);
        field->add_wall(wall);
    }
    {
        std::vector<glm::vec2> wall;
        wall.emplace_back(0.3, -0.3);
        wall.emplace_back(0.3 - wall_thickness, -0.3);
        wall.emplace_back(0.3, 0.3);
        wall.emplace_back(0.3 - wall_thickness, 0.3);
        field->add_wall(wall);
    }

    std::unique_ptr<glgui::view::Field> fieldView = std::make_unique<glgui::view::Field>(field.get());
    std::unique_ptr<glgui::view::Observer> observerView = std::make_unique<glgui::view::Observer>(bg_color);
    std::unique_ptr<glgui::view::Trajectory> trajectoryView = std::make_unique<glgui::view::Trajectory>();
    std::unique_ptr<glgui::view::Map> mapView = std::make_unique<glgui::view::Map>();

    std::unique_ptr<glgui::component::Parameters> parameters = std::make_unique<glgui::component::Parameters>(50, 0.5, 10);
    std::unique_ptr<glgui::frame::Main> mainFrame = std::make_unique<glgui::frame::Main>(parameters.get());

    int time_step = 0;
    std::vector<GLfloat> trajectory; // オドメトリによる軌跡
    std::vector<GLfloat> map; // 生成された地図
    std::vector<glm::vec4> scans; // レーザースキャナから得られた値

    double round_angle_rad = M_PI_2;
    glm::vec2 last_scan_location;
    bool is_first_scan = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        int screen_width, screen_height;
        glfwGetFramebufferSize(window, &screen_width, &screen_height);
        glViewport(0, 0, screen_width, screen_height);
        glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int square_length = screen_width / 3.0;
        fieldView->render(window, 0, 0, square_length, square_length);

        if (mainFrame->_is_slam_running) {
            if (is_first_scan) {
                is_first_scan = false;
                time_step = 0;
                map.clear();
                trajectory.clear();
                round_angle_rad = M_PI_2;
            }
            int num_beams = parameters->_num_beams;
            double cursor_x, cursor_y;
            glfwGetCursorPos(window, &cursor_x, &cursor_y);
            double round_base_angle_rad = M_PI / 100.0f * parameters->_speed;
            round_angle_rad -= round_base_angle_rad;
            double moving_radius = 0.75f;

            // オドメトリによる位置予測
            if (trajectory.size() == 0) {
                double initial_location_x = moving_radius * cos(round_angle_rad);
                double initial_location_y = moving_radius * sin(round_angle_rad);
                trajectory.emplace_back(initial_location_x);
                trajectory.emplace_back(initial_location_y);
            } else {
                double prev_location_x = trajectory[trajectory.size() - 2];
                double prev_location_y = trajectory[trajectory.size() - 1];
                double delta_distance = sin(round_base_angle_rad / 2.0) * moving_radius * 2.0;
                double delta_angle = -(M_PI_2 - (M_PI - round_base_angle_rad) / 2.0);
                double delta_moving_distance_x = cos(delta_angle) * delta_distance;
                double delta_moving_distance_y = sin(delta_angle) * delta_distance;
                double transform_angle_rad = -(M_PI_2 - round_angle_rad);
                double next_location_x = cos(transform_angle_rad) * delta_moving_distance_x - sin(transform_angle_rad) * delta_moving_distance_y + prev_location_x;
                double next_location_y = sin(transform_angle_rad) * delta_moving_distance_x + cos(transform_angle_rad) * delta_moving_distance_y + prev_location_y;
                trajectory.emplace_back(next_location_x);
                trajectory.emplace_back(next_location_y);
            }

            // ロボットの移動
            glm::vec2 location;
            location.x = trajectory[trajectory.size() - 2];
            location.y = trajectory[trajectory.size() - 1];

            // レーザースキャナによる観測
            if (time_step % parameters->_laser_scanner_interval == 0) {
                if (scans.size() != parameters->_num_beams) {
                    scans.resize(parameters->_num_beams);
                }
                for (int n = 0; n < num_beams; n++) {
                    scans[n] = glm::vec4(0, 0, 0, 0);
                }
                double robot_angle_rad = round_angle_rad - M_PI_2;
                observer->observe(field.get(), location, robot_angle_rad, num_beams, scans);
                last_scan_location.x = location.x;
                last_scan_location.y = location.y;
                // 地図構築
                for (int n = 0; n < num_beams; n++) {
                    // 座標変換
                    glm::vec4& scan = scans[n];
                    double distance = scan[2];
                    double angle_rad = scan[3];
                    glm::vec2 point;
                    if (parameters->_odometry_enabled) {
                        // 座標系を変換
                        angle_rad += robot_angle_rad;
                        point.x = cos(angle_rad) * distance + location.x;
                        point.y = sin(angle_rad) * distance + location.y;
                    } else {
                        point.x = cos(angle_rad) * distance;
                        point.y = sin(angle_rad) * distance;
                    }
                    map.emplace_back(point.x);
                    map.emplace_back(point.y);
                }
            }

            time_step++;
        } else {
            is_first_scan = true;
        }

        mapView->render(window, square_length * 2, 0, square_length, square_length, map);
        observerView->render(window, square_length, 0, square_length, square_length, last_scan_location, scans);
        if (parameters->_odometry_enabled) {
            trajectoryView->render(window, square_length * 2, 0, square_length, square_length, trajectory);
        }
        mainFrame->render();

        glViewport(0, 0, screen_width, screen_height);
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
