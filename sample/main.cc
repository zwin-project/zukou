#include <linux/input.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <zukou/zukou.h>

#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <memory>

#include "cube.h"

class WindowA : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<WindowA> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
    return std::shared_ptr<WindowA>(new WindowA(app, half_size));
  }

  void Init() {
    auto self = shared_from_this();
    object_group_.reset(new zukou::objects::ObjectGroup());
    cube1_.reset(new Cube(app_, self));
    cube2_.reset(new Cube(app_, self));
    cube1_->Init();
    cube2_->Init();
    object_group_->AddObject(cube1_);
    object_group_->AddObject(cube2_);
  }

 private:
  WindowA(std::shared_ptr<zukou::Application> app, glm::vec3 half_size)
      : CuboidWindow(app, half_size) {}

  virtual bool Frame() override { return object_group_->Draw(); };

  void RayButton([[maybe_unused]] uint32_t serial,
      [[maybe_unused]] uint32_t time, [[maybe_unused]] uint32_t button,
      bool pressed) override {
    if (pressed && button == BTN_LEFT) {
      this->Move(serial);
    } else {
      object_group_->RayButton(serial, time, button, pressed);
    }
  }

  virtual void RayMotion(
      uint32_t time, glm::vec3 origin, glm::vec3 direction) override {
    object_group_->RayMotion(origin, direction, time);
  }

  void RayLeave([[maybe_unused]] uint32_t serial) override {
    object_group_->RayLeave();
  }

  virtual void DataDeviceEnter(uint32_t serial,
      [[maybe_unused]] glm::vec3 origin, [[maybe_unused]] glm::vec3 direction,
      std::weak_ptr<zukou::DataOffer> data_offer) override {
    object_group_->DataDeviceEnter(serial, data_offer);
  }

  virtual void DataDeviceLeave() override { object_group_->DataDeviceLeave(); }

  virtual void DataDeviceMotion(
      uint32_t time, glm::vec3 origin, glm::vec3 direction) override {
    object_group_->DataDeviceMotion(time, origin, direction);
  }

  virtual void DataDeviceDrop() override { object_group_->DataDeviceDrop(); }

  void Configured([[maybe_unused]] uint32_t serial) override {
    float min_half =
        half_size().x > half_size().y
            ? (half_size().y > half_size().z ? half_size().z : half_size().y)
            : (half_size().x > half_size().z ? half_size().z : half_size().x);
    cube1_->set_position(glm::vec3(0, 0.05f, 0.0f));
    cube1_->set_half_size(glm::vec3(min_half / 1.8f));
    cube1_->set_quaternion(glm::quat(glm::vec3(M_PI / 6, M_PI / 9, 0)));
    cube2_->set_position(glm::vec3(0, -0.08f, 0.0f));
    cube2_->set_half_size(
        glm::vec3(min_half / 2.1f, min_half / 4, min_half / 3.3));
    cube2_->set_quaternion(glm::quat(glm::vec3(-M_PI / 6, -M_PI / 9, 0)));
    this->ScheduleNextFrame();
  }

  std::unique_ptr<zukou::objects::ObjectGroup> object_group_;
  std::shared_ptr<Cube> cube1_;
  std::shared_ptr<Cube> cube2_;
};

class WindowB : public zukou::CuboidWindow {
 public:
  static std::shared_ptr<WindowB> Create(
      std::shared_ptr<zukou::Application> app, glm::vec3 half_size) {
    return std::shared_ptr<WindowB>(new WindowB(app, half_size));
  }

  void Init() {
    auto self = shared_from_this();
    cube1_.reset(new zukou::primitives::FrameCuboid(app_, self));
    cube2_.reset(new zukou::entities::FrameCuboid(app_, self));
    cube1_->Init();
    cube2_->Init();
  }

 private:
  WindowB(std::shared_ptr<zukou::Application> app, glm::vec3 half_size)
      : CuboidWindow(app, half_size) {}

  void RayButton([[maybe_unused]] uint32_t serial,
      [[maybe_unused]] uint32_t time, [[maybe_unused]] uint32_t button,
      bool pressed) override {
    if (pressed) this->Move(serial);
  }

  virtual bool Frame() override {
    bool should_commit = false;
    if (cube1_->Draw()) should_commit = true;
    if (cube2_->Draw()) should_commit = true;
    return should_commit;
  };

  void Configured([[maybe_unused]] uint32_t serial) override {
    float min_half =
        half_size().x > half_size().y
            ? (half_size().y > half_size().z ? half_size().z : half_size().y)
            : (half_size().x > half_size().z ? half_size().z : half_size().x);
    cube1_->set_position(glm::vec3(-0.03, 0.0f, 0.0f));
    cube1_->set_half_size(glm::vec3(min_half / 3.5f));
    cube1_->set_quaternion(glm::quat(glm::vec3(-M_PI / 10, 0, M_PI / 8)));
    cube1_->set_frame_color(cyan);

    cube2_->set_position(glm::vec3(0, -0.05f, 0.02f));
    cube2_->set_half_size(
        glm::vec3(min_half / 2.2f, min_half / 3.2, min_half / 3.2));
    cube2_->set_quaternion(glm::quat(glm::vec3(0, -M_PI / 8, M_PI / 10)));
    cube2_->set_frame_color(cyan);
    this->ScheduleNextFrame();
  }

  void RayEnter([[maybe_unused]] uint32_t serial,
      [[maybe_unused]] glm::vec3 origin,
      [[maybe_unused]] glm::vec3 direction) override {
    cube2_->set_frame_color(magenta);
    this->ScheduleNextFrame();
  }

  virtual void RayMotion([[maybe_unused]] uint32_t time, glm::vec3 origin,
      glm::vec3 direction) override {
    if (cube2_->Intersect(origin, direction) > 0)
      cube2_->set_frame_color(yellow);
    else
      cube2_->set_frame_color(magenta);

    this->ScheduleNextFrame();
  }

  void RayLeave([[maybe_unused]] uint32_t serial) override {
    cube2_->set_frame_color(cyan);
    this->ScheduleNextFrame();
  }

  std::unique_ptr<zukou::primitives::FrameCuboid> cube1_;
  std::unique_ptr<zukou::entities::FrameCuboid> cube2_;
};

class SignalEvent : public zukou::PollEvent {
 public:
  SignalEvent(std::weak_ptr<zukou::Application> app) : app_(app) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    fd_ = signalfd(-1, &mask, 0);
    op_ = EPOLL_CTL_ADD;
    epoll_event_.events = EPOLLIN;
  }

  virtual bool Done([[maybe_unused]] struct epoll_event *ev) override final {
    auto app = app_.lock();
    app->Terminate(EXIT_SUCCESS);
    return true;
  }

  ~SignalEvent() { close(fd_); }

  std::weak_ptr<zukou::Application> app_;
};

int main() {
  std::shared_ptr<zukou::Application> app = zukou::Application::Create();
  std::shared_ptr<WindowA> window_a;
  std::shared_ptr<WindowB> window_b;
  std::shared_ptr<SignalEvent> sig_event(new SignalEvent(app));

  app->Connect("zigen-0");

  // FrameCuboid Window / Virtual Object must be managed by shared_ptr.
  window_a = WindowA::Create(app, glm::vec3(0.25));
  window_b = WindowB::Create(app, glm::vec3(0.15));
  window_a->Init();
  window_b->Init();

  app->AddPollEvent(sig_event);

  int ret = app->Run();

  std::cout << "exit (" << ret << ")" << std::endl;

  return ret;
}
