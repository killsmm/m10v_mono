/*
 * u-boot/drivers/mmc/f_sdh30_sdhci.c
 *
 * Copyright (C) 2011-2012 FUJITSU SEMICONDUCTOR LIMITED
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mmc.h"
#include "sdhci.h"

static struct sdhci_host f_sdh30_host[] = { CONFIG_F_SDH30_HOSTS_INIT };

int f_sdh30_init(int dev_idx)
{
	f_sdh30_host[dev_idx].version = sdhci_readw(&f_sdh30_host[dev_idx], SDHCI_HOST_VERSION);

	return add_sdhci(&f_sdh30_host[dev_idx], f_sdh30_host[dev_idx].max_clk, 400000);
}
