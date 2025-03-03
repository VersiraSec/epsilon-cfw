#ifndef STATISTICS_HISTOGRAM_CONTROLLER_H
#define STATISTICS_HISTOGRAM_CONTROLLER_H

#include <escher/button_row_controller.h>
#include "store.h"
#include "multiple_data_view_controller.h"
#include "multiple_histograms_view.h"

namespace Statistics {

class HistogramController : public MultipleDataViewController, public Escher::ButtonRowDelegate {

public:
  HistogramController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Escher::ButtonRowController * header, Store * store, uint32_t * m_storeVersion, uint32_t * m_barVersion, uint32_t * m_rangeVersion, int * m_selectedBarIndex, int * selectedSeriesIndex);

  HistogramParameterController * histogramParameterController() { return &m_histogramParameterController; }
  void setCurrentDrawnSeries(int series);
  Escher::StackViewController * stackController();

  int numberOfButtons(Escher::ButtonRowController::Position position) const override { return isEmpty() ? 0 : 1; }
  Escher::Button * buttonAtIndex(int index, Escher::ButtonRowController::Position position) const override;

  // ViewController
  const char * title() override;
  void viewWillAppear() override;
  MultipleDataView * multipleDataView() override { return &m_view; }
  TELEMETRY_ID("Histogram");

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * firstResponder) override;
  void willExitResponderChain(Escher::Responder * nextFirstResponder) override;
private:
  constexpr static int k_maxNumberOfBarsPerWindow = 100;
  constexpr static int k_maxIntervalLegendLength = 33;
  constexpr static int k_maxLegendLength = 13;
  constexpr static int k_maxNumberOfCharacters = 30;
  void highlightSelection() override;
  Escher::Responder * tabController() const override;
  void reloadBannerView() override;
  void preinitXRangeParameters(double * xMin);
  void initRangeParameters();
  void initYRangeParameters(int series);
  void initBarParameters();
  void initBarSelection();
  // return true if the window has scrolled
  bool moveSelectionHorizontally(int deltaIndex) override;
  MultipleHistogramsView m_view;
  uint32_t * m_storeVersion;
  uint32_t * m_barVersion;
  uint32_t * m_rangeVersion;
  HistogramParameterController m_histogramParameterController;
  Escher::Button m_parameterButton;
};

}


#endif
