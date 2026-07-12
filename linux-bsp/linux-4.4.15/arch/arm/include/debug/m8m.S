/**
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

	.macro addruart, rp, rv, tmp
	ldr	\rp, =CONFIG_DEBUG_UART_PHYS
	ldr	\rv, =CONFIG_DEBUG_UART_VIRT
	.endm

	.macro senduart,rd,rx
	strh    \rd, [\rx, #0x04]               @ write to TxData register
	.endm

	.macro waituart,rd,rx
1:	ldrb    \rd, [\rx, #0x03]               @ SSR
	tst     \rd, #1 << 1                    @ check TDRE bit
	beq     1b
	.endm

	.macro busyuart,rd,rx
2:	ldrb    \rd, [\rx, #0x03]               @ SSR
	tst     \rd, #1 << 1                    @ check TEMT bit
	beq     2b
	.endm
