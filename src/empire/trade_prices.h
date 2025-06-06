#ifndef EMPIRE_TRADE_PRICES_H
#define EMPIRE_TRADE_PRICES_H

#include "core/buffer.h"
#include "game/resource.h"

/**
 * @file
 * Trade prices.
 */

/**
 * Reset trade prices to the default
 */
void trade_prices_reset(void);

/**
 * Get the base buy price for the resource, without any discounts or penalties applied
 * @param resource Resource
 */
int trade_price_base_buy(resource_type resource);

/**
 * Get the buy price for the resource
 * @param resource Resource
 * @param land_trader 1 if land_trader 0 else sea_trader
 */
int trade_price_buy(resource_type resource, int land_trader);

/**
 * Get the base sell price for the resource, without any discounts or penalties applied
 * @param resource Resource
 */
int trade_price_base_sell(resource_type resource);

/**
 * Get the sell price for the resource
 * @param resource Resource
 * @param land_trader 1 if land_trader 0 else sea_trader
 */
int trade_price_sell(resource_type resource, int land_trader);

/**
 * Determine the direction of price change for a resource trade route.
 * Returns -1 if the price has decreased, 1 if it has increased, or 0 if unchanged,
 * based on the trade factor relative to the base price.
 *
 * @param land_trader 1 if the trade route is land-based, 0 for sea-based
 * @param is_sell 1 if checking the sell price, 0 for buy price
 * @return -1 if price decreased, 1 if price increased, 0 if no change
 */

int trade_factor_sign(int land_trader, int is_sell);

/**
 * Change the trade price for resource by amount
 * @param resource Resource to change
 * @param amount Amount to change, can be positive or negative
 * @return True if the price has been changed
 */
int trade_price_change(resource_type resource, int amount);

/**
 * Change only the buy price of a resource
 * @param resource Resource to change
 * @param new_price The new price to set it to
 * @return True if the price has been changed
 */
int trade_price_set_buy(resource_type resource, int new_price);

/**
 * Change only the sell price of a resource
 * @param resource Resource to change
 * @param new_price The new price to set it to
 * @return True if the price has been changed
 */
int trade_price_set_sell(resource_type resource, int new_price);

/**
 * Save trade prices to buffer
 * @param buf Buffer
 */
void trade_prices_save_state(buffer *buf);

/**
 * Load trade prices from buffer
 * @param buf Buffer
 */
void trade_prices_load_state(buffer *buf);

#endif // EMPIRE_TRADE_PRICES_H
