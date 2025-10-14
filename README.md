# GPIO device driver - Minimal

A minimal POC code for a GPIO device driver.

To simulate a GPIO, we can leverage `gpio-mockup` or `gpio-sim` from kernel.

## 1. `gpio-mockup` driver

## 1-1. Build and Run

```bash
make
sudo imsmod gpio-mockup.ko gpio_mockup_ranges=-1,8
```

## 1-2. Check if GPIOs is simulated correctly

```bash
sudo cat /sys/kernel/debug/gpio
```

Example output:
```
gpiochip0: GPIOs 512-519, parent: platform/gpio-mockup.0, gpio-mockup-A:
```

The GPIO number 512 to 519 can be used in the poc code: `poc/gpio-minimal.c`.



## 1-3. Make sure which line you want to trigger

```bash
gpioinfo
```

Example output:
```
gpiochip0 - 8 lines:
        line   0:      unnamed       unused   input  active-high
        line   1:      unnamed       unused   input  active-high
        line   2:      unnamed       unused   input  active-high
        line   3:      unnamed       unused   input  active-high
        line   4:      unnamed       unused   input  active-high
        line   5:      unnamed       unused   input  active-high
        line   6:      unnamed       unused   input  active-high
        line   7:      unnamed       unused   input  active-high
```

Mapping from GPIO number to line in chip:

|GPIO number| line |
|-----------|------|
|512        |0     |
|513        |1     |
|514        |2     |
|515        |3     |
|516        |4     |
|517        |5     |
|518        |6     |
|519        |7     |

## 1-4. Load our POC driver to trigger ISR

```bash
sudo insmod poc/gpio-minimal.ko
```

## 1-5. Trigger 

Suppose I want to trigger for GPIO 512:

```bash
echo 1 > /sys/kernel/debug/gpio-mockup/gpiochip0/0
echo 0 > /sys/kernel/debug/gpio-mockup/gpiochip0/0
```

## 1-6. Verify

```bash
sudo rmmod gpio_minimal
```

If you see `var: 1` in the dmesg output, that's means our ISR is triggered.

---

## 2. `gpio-sim` driver

- recommened
- more-flexible interface

## 2-1. Build and Run

```bash
make
sudo imsmod gpio-sim.ko
```

## 2-2. Setup

```bash
# 1. Create chip directory
mkdir /sys/kernel/config/gpio-sim/test

# 2. Create a bank — the directory name becomes part of the chip label
mkdir /sys/kernel/config/gpio-sim/test/bank0

# 3. Set number of lines
echo 8 > /sys/kernel/config/gpio-sim/test/bank0/num_lines

# 4. Create per-line directories BEFORE going live
mkdir /sys/kernel/config/gpio-sim/test/bank0/line0

# 5. Activate the chip
echo 1 > /sys/kernel/config/gpio-sim/test/live
```

## 2-2. Check if GPIOs is simulated correctly

```bash
sudo cat /sys/kernel/debug/gpio
```

Example output:
```
gpiochip1: GPIOs 520-520, parent: platform/gpio-sim.0, gpio-sim.0-node0, can sleep:
```

The GPIO number 520 can be used in the poc code: `poc/gpio-minimal.c`.

---

## 2-3. Make sure which line you want to trigger

```bash
gpioinfo
```

Example output:
```
gpiochip1 - 1 lines:
        line   0:      unnamed       unused   input  active-high
```

Mapping from GPIO number to line in chip:

|GPIO number| line |
|-----------|------|
|520        |0     |


## 2-4. Load our POC driver to trigger ISR

```bash
sudo insmod poc/gpio-minimal.ko
```

## 2-5. Trigger 

Suppose I want to trigger for GPIO 520:

```bash
echo 'pull-up' > /sys/devices/platform/gpio-sim.0/gpiochip1/sim_gpio0/pull
```

## 2-6. Verify

```bash
sudo rmmod gpio_minimal
```

If you see `var: 1` in the dmesg output, that's means our ISR is triggered.

---

Refs:
- https://kernel.org/doc/Documentation/admin-guide/gpio/gpio-mockup.rst
- https://kernel.org/doc/Documentation/admin-guide/gpio/gpio-sim.rst
