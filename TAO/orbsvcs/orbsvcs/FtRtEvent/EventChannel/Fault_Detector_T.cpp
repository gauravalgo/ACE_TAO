/// $Id$
#include "../Utils/Log.h"

template <class ACCEPTOR, class CONNECTOR, class DETECTION_HANDLER>
Fault_Detector_T<ACCEPTOR, CONNECTOR, DETECTION_HANDLER>::Fault_Detector_T()
{
}

template <class ACCEPTOR, class CONNECTOR, class DETECTION_HANDLER>
Fault_Detector_T<ACCEPTOR, CONNECTOR, DETECTION_HANDLER>::~Fault_Detector_T()
{
}


template <class ACCEPTOR, class CONNECTOR, class DETECTION_HANDLER>
int
Fault_Detector_T<ACCEPTOR, CONNECTOR, DETECTION_HANDLER>::init_acceptor()
{
  ACE_INET_Addr listen_addr;
  if (acceptor_.open(listen_addr, &reactor_task_.reactor_) != 0)
    ACE_ERROR_RETURN((LM_ERROR, "Connot open acceptor\n"), -1);

  if (acceptor_.acceptor().get_local_addr(listen_addr) !=0)
    ACE_ERROR_RETURN((LM_ERROR, "Connot get local addr\n"), -1);

  TAO_FTRTEC::Log(1, "listening at %s:%d\n", listen_addr.get_host_name(),
                                               listen_addr.get_port_number());
  char* buf = CORBA::string_alloc(MAXHOSTNAMELEN);
  listen_addr.addr_to_string(buf, MAXHOSTNAMELEN, 0);
  location_.length(1);
  location_[0].id = buf;
  return 0;
}

template <class ACCEPTOR, class CONNECTOR, class DETECTION_HANDLER>
int
Fault_Detector_T<ACCEPTOR, CONNECTOR, DETECTION_HANDLER>::connect(const FTRT::Location& location)
{
    if (location.length() == 0)
      return -1;

    ACE_INET_Addr prev_addr(location[0].id);

    DETECTION_HANDLER* handler;
    ACE_NEW_RETURN(handler, DETECTION_HANDLER(listener_), 0) ;

    handler->reactor(&reactor_task_.reactor_);
    TAO_FTRTEC::Log(1, "connecting to %s\n", location[0].id.in());

    int result = connector_.connect(handler->peer(), prev_addr);

    if (result == 0)
      handler->open(this);
    else
      handler->close();

    return result ;
}
