/**
 * @file Supplier.cpp
 *
 * $Id$
 *
 * @author Carlos O'Ryan <coryan@uci.edu>
 */

#include "Supplier.h"
#include "Implicit_Deactivator.h"

ACE_RCSID(TAO_PERF_RTEC, Supplier, "$Id$")

Supplier::Supplier (CORBA::Long experiment_id,
                    CORBA::Long event_type,
                    CORBA::Long event_range,
                    PortableServer::POA_ptr poa)
  : experiment_id_ (experiment_id)
  , event_type_ (event_type)
  , event_range_ (event_range)
  , default_POA_ (PortableServer::POA::_duplicate (poa))
{
}

void
Supplier::connect (RtecEventChannelAdmin::EventChannel_ptr ec
                   ACE_ENV_ARG_DECL)
{
  RtecEventChannelAdmin::SupplierAdmin_var supplier_admin =
    ec->for_suppliers (ACE_ENV_SINGLE_ARG_PARAMETER);
  ACE_CHECK;

  {
    ACE_GUARD (TAO_SYNCH_MUTEX, ace_mon, this->mutex_);
    if (!CORBA::is_nil (this->proxy_consumer_.in ()))
      return;

    this->proxy_consumer_ =
      supplier_admin->obtain_push_consumer (ACE_ENV_SINGLE_ARG_PARAMETER);
    ACE_CHECK;
  }

  RtecEventComm::PushSupplier_var supplier =
    this->_this (ACE_ENV_SINGLE_ARG_PARAMETER);
  ACE_CHECK;

  RtecEventChannelAdmin::SupplierQOS supplier_qos;
  supplier_qos.is_gateway = 0;
  supplier_qos.publications.length (this->event_range_);
  for (CORBA::Long i = 0; i != this->event_range_; ++i)
    {
      RtecEventComm::EventHeader& sh =
        supplier_qos.publications[i].event.header;
      sh.type   = this->event_type_ + 2 * i;
      sh.source = this->experiment_id_;
    }

  this->proxy_consumer_->connect_push_supplier (supplier.in (),
                                                supplier_qos
                                                ACE_ENV_ARG_PARAMETER);
  ACE_CHECK;
}

void
Supplier::disconnect (ACE_ENV_SINGLE_ARG_DECL)
{
  RtecEventChannelAdmin::ProxyPushConsumer_var proxy;
  {
    ACE_GUARD (TAO_SYNCH_MUTEX, ace_mon, this->mutex_);
    if (CORBA::is_nil (this->proxy_consumer_.in ()))
      return;
    proxy = this->proxy_consumer_._retn ();
  }

  Implicit_Deactivator deactivator (this
                                    ACE_ENV_ARG_PARAMETER);
  ACE_CHECK;

  ACE_TRY
    {
      proxy->disconnect_push_consumer (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY {} ACE_ENDTRY;
}

void
Supplier::push (const RtecEventComm::EventSet &events
                     ACE_ENV_ARG_DECL)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  RtecEventChannelAdmin::ProxyPushConsumer_var proxy;
  {
    ACE_GUARD (TAO_SYNCH_MUTEX, ace_mon, this->mutex_);
    if (CORBA::is_nil (this->proxy_consumer_.in ()))
      return;
    proxy = this->proxy_consumer_;
  }

  proxy->push (events ACE_ENV_ARG_PARAMETER);
}

void
Supplier::disconnect_push_supplier (ACE_ENV_SINGLE_ARG_DECL_NOT_USED)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_GUARD (TAO_SYNCH_MUTEX, ace_mon, this->mutex_);
  this->proxy_consumer_ =
    RtecEventChannelAdmin::ProxyPushConsumer::_nil ();
}

PortableServer::POA_ptr
Supplier::_default_POA (ACE_ENV_SINGLE_ARG_DECL_NOT_USED)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  return PortableServer::POA::_duplicate (this->default_POA_.in ());
}
