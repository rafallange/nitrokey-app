#ifndef NITROKEYAPP_THREADWORKER_H
#define NITROKEYAPP_THREADWORKER_H

#include <QObject>
#include <functional>
#include <QThread>
#include <QString>
#include <QMutex>
#include <QVariant>
#include <QMap>
#include <memory>


/***
 * declaring data transporting type
 * alias cannot be used inside slots/signals for
 * some reason (they are not connected with this way)
 */
using Data = QMap<QString, QVariant>;

namespace ThreadWorkerNS {
  class Worker : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(Worker)

  public:
    Worker(QObject *parent, const std::function<Data()> &datafunc);

  public slots:
    void fetch_data();

  signals:
    void finished(QMap<QString, QVariant> data);
    void finished();

  private:
    QMutex mutex;
    std::function<Data()> datafunc;
  };
}

/***
 * Allocate on heap and add parent (to delete on its destruction)
 */
class ThreadWorker : public QObject {
  using Worker = ThreadWorkerNS::Worker;
  Q_OBJECT
  Q_DISABLE_COPY(ThreadWorker)

public:
  /**
   * A class for non-blocking update of UI with data requested from device.
   * @param datafunc returns Data, instructions for requesting data from device,
   * will be run on separate thread
   * @param usefunc accepts Data, instructions for updating GUI with data received
   * from worker thread
   * @param parent pointer to parent - this will cause object destruction
   * on parent's destruction
   */
  ThreadWorker(const std::function<Data()> &datafunc,
               const std::function<void(Data)> &usefunc,
               QObject *parent = nullptr, std::string name = "");

  ~ThreadWorker();

private slots:
  void worker_finished();
  void use_data(QMap<QString, QVariant> data);

private:
  std::shared_ptr<ThreadWorkerNS::Worker> worker;
  QThread *worker_thread;
  std::function<void(Data)> usefunc;
  QMutex mutex;

  void stop_thread();

    std::string name;
};

//template:
//ThreadWorker *tw = new ThreadWorker(
//    []() -> Data {
//      Data data;
//
//      return data;
//    },
//    [this](Data data){
//
//    }, this);



#endif //NITROKEYAPP_THREADWORKER_H
