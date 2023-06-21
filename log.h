#pragma once

#define LOG_DEBUG(...) do { printk(KERN_DEBUG __VA_ARGS__); } while (0)
#define LOG_INFO(...) do { printk(KERN_INFO __VA_ARGS__); } while (0)
#define LOG_WARN(...) do { printk(KERN_WARNING __VA_ARGS__); } while (0)
#define LOG_ERROR(...) do { printk(KERN_ERR __VA_ARGS__); } while (0)
