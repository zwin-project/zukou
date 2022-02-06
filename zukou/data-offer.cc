#include <fcntl.h>
#include <unistd.h>
#include <zigen-client-protocol.h>
#include <zukou/zukou.h>

namespace zukou {

std::shared_ptr<DataOffer> DataOffer::Create(
    struct zgn_data_offer *id, std::shared_ptr<Application> app) {
  return std::shared_ptr<DataOffer>(new DataOffer(id, app));
}

DataOffer::DataOffer(
    struct zgn_data_offer *id, std::shared_ptr<Application> app)
    : app_(app), proxy_(id) {
  zgn_data_offer_add_listener(proxy_, &DataOffer::data_offer_listener_, this);
}

DataOffer::~DataOffer() { zgn_data_offer_destroy(proxy_); }

void DataOffer::Receive(
    std::string mime_type, std::function<void(int fd)> callback) {
  int p[2];
  if (pipe2(p, O_CLOEXEC) == -1) return;

  zgn_data_offer_receive(proxy_, mime_type.c_str(), p[1]);
  close(p[1]);

  app_->AddPollEvent(std::make_shared<ReceivePollEvent>(p[0], callback));
}

void DataOffer::Accept(uint32_t serial, std::string mime_type) {
  zgn_data_offer_accept(proxy_, serial, mime_type.c_str());
}

void DataOffer::SetActions(uint32_t dnd_actions, uint32_t preferred_action) {
  zgn_data_offer_set_actions(proxy_, dnd_actions, preferred_action);
}

const struct zgn_data_offer_listener DataOffer::data_offer_listener_ = {
    DataOffer::Offer,
    DataOffer::SourceActions,
    DataOffer::Action,
};

void DataOffer::Offer(void *data,
    [[maybe_unused]] struct zgn_data_offer *zgn_data_offer,
    const char *mime_type) {
  DataOffer *data_offer = reinterpret_cast<DataOffer *>(data);
  data_offer->mime_types_.push_back(mime_type);
}

void DataOffer::SourceActions(void *data,
    [[maybe_unused]] struct zgn_data_offer *zgn_data_offer,
    uint32_t source_actions) {
  DataOffer *data_offer = reinterpret_cast<DataOffer *>(data);
  data_offer->source_actions_ = source_actions;
}

void DataOffer::Action(void *data,
    [[maybe_unused]] struct zgn_data_offer *zgn_data_offer,
    uint32_t dnd_action) {
  DataOffer *data_offer = reinterpret_cast<DataOffer *>(data);
  data_offer->dnd_action_ = dnd_action;
}

DataOffer::ReceivePollEvent::ReceivePollEvent(
    int fd, std::function<void(int fd)> callback) {
  fd_ = fd;
  op_ = EPOLL_CTL_ADD;
  epoll_event_.events = EPOLLIN;
  callback_ = callback;
}

bool DataOffer::ReceivePollEvent::Done(
    [[maybe_unused]] struct epoll_event *ev) {
  callback_(fd_);
  return true;
}

}  // namespace zukou
