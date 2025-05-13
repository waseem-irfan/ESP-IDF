#include "dw3000_hw.h"
#include "dwhw.h"
#include "dwmac.h"
#include "dwphy.h"
#include "dwproto.h"
#include "ranging.h"
#include "esp_log.h"
// #include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#define LOG_TAG "UWB_TAG"

static const uint16_t PANID = 7766;
static const uint16_t MAC16 = 0xAABB;

static void twr_done_cb(uint64_t src, uint64_t dst, uint16_t dist,
						 uint16_t num)
{
  esp_rom_printf("TWR Done %04X: %u cm\n", (uint16_t)dst, dist);
}

void test_twr(void)
{
  // decadriver init
  dw3000_hw_init();
  dw3000_hw_reset();
  dw3000_hw_init_interrupt();

  // libdeca init
  dwhw_init();
  dwphy_config();
  dwphy_set_antenna_delay(DWPHY_ANTENNA_DELAY);
  dwmac_init(PANID, MAC16, dwprot_rx_handler, NULL, NULL);
  dwmac_set_frame_filter();
  twr_init(TWR_PROCESSING_DELAY, 1);
  twr_set_observer(twr_done_cb);
  // two way ranging to 0x0001
  twr_start(0x0001);
  // heap_caps_check_integrity_all();
}

void app_main(void){
  test_twr();
}