#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpStream.h"

#include <thread>

// a thread-per-connection current echo server
int main()
{
  InetAddress listenAddr(3007);
  Acceptor acceptor(listenAddr);
  printf("Accepting... Ctrl-C to exit\n");
  int count = 0;
  while (true)
  {
    TcpStreamPtr tcpStream = acceptor.accept();
    printf("accepted no. %d client\n", ++count);

    // C++11 doesn't allow capturing unique_ptr in lambda, C++14 allows.
    std::thread thr([count] (TcpStreamPtr stream) {
      printf("thread for no. %d client started.\n", count);
      char buf[4096];
      int nr = 0;
      while ( (nr = stream->receiveSome(buf, sizeof(buf))) > 0)
      {
        int nw = stream->sendAll(buf, nr);
        if (nw < nr)
        {
          break;
        }
      }
      printf("thread for no. %d client ended.\n", count);
    }, std::move(tcpStream));
    thr.detach();
  }
}
