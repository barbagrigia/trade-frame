/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

/*
 * File:    Combo.h
 * Author:  raymond@burkholder.net
 * Project: TFOptions
 * Created on June 7, 2019, 5:08 PM
 */

#ifndef COMBO_H
#define COMBO_H

#include <vector>

#include <OUCharting/ChartDataView.h>

#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>

#include "IvAtm.h"
#include "SpreadValidation.h"

#include "Leg.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

struct ConstructionTools {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using fConstructedOption_t = std::function<void(pOption_t)>;
  using fConstructOption_t = std::function<void(const std::string&, const pInstrument_t, fConstructedOption_t)>;  // source from IQFeed Symbol Name

  boost::gregorian::date m_dateExpiry;
  IvAtm& m_chains;
  pWatch_t m_pWatchUnderlying;
  fConstructOption_t& m_fConstructOption;

  ConstructionTools(
    boost::gregorian::date dateExpiry,
    IvAtm& chains,
    pWatch_t pWatchUnderlying,
    fConstructOption_t& fConstructOption
    )
  : m_dateExpiry( dateExpiry ), m_chains( chains ),
    m_pWatchUnderlying( pWatchUnderlying ),
    m_fConstructOption( fConstructOption )
  {}
};


class Combo {
public:

  using idPortfolio_t = ou::tf::Portfolio::idPortfolio_t;

  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  using EColour = ou::Colour::enumColour;
  using EOptionSide = ou::tf::OptionSide::enumOptionSide;
  using EOrderSide = ou::tf::OrderSide::enumOrderSide;

  using fBuildPositionCallBack_t
    = std::function<void(pPosition_t,pChartDataView_t,EColour)>;
  using fBuildLeg_t
    = std::function<void(const idPortfolio_t&, EOptionSide, double, fBuildPositionCallBack_t&&)>;

  enum class State { Initializing, Positions, Executing, Watching, Canceled, Closing };
  State m_state;

  Combo( );
  Combo( const Combo& rhs ) = delete;
  Combo& operator=( const Combo& rhs ) = delete;
  Combo( Combo&& rhs );
  virtual ~Combo( );

  void SetPortfolio( pPortfolio_t );
  pPortfolio_t GetPortfolio() { return m_pPortfolio; }

  void AddPosition( pPosition_t, pChartDataView_t pChartData, ou::Colour::enumColour );

  virtual void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt );

  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide ) = 0;

  double GetNet( double price );

  void CloseForProfits( double price );
  void TakeProfits( double price );
  void CloseExpiryItm( double price, const boost::gregorian::date date );
  void CloseFarItm( double price );
  bool CloseItmLeg( double price );
  bool CloseItmLegForProfit( double price );

  void CancelOrders();
  void ClosePositions();

  bool AreOrdersActive() const;
  void SaveSeries( const std::string& sPrefix );

protected:

  static const double m_dblTwentyPercent;
  static const double m_dblMaxStrikeDelta;
  static const double m_dblMaxStrangleDelta;

  SpreadValidation m_SpreadValidation;

  pPortfolio_t m_pPortfolio; // positions need to be associated with portfolio

  using vLeg_t = std::vector<ou::tf::Leg>;
  vLeg_t m_vLeg;

private:

  void Update( bool bTrending, double dblPrice );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* COMBO_H */

