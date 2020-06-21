/*#ifndef __UTILS__
#define __UTILS__

#define ARP_BROAD_REQ   1
#define ARP_REPLY       2
#define ARP_MSG         806
#define BROADCAST_MAC   0xFFFFFFFFFFFF

#define IS_MAC_BROADCAST_ADDR(mac)   \
    (mac[0] == 0xFF  &&  mac[1] == 0xFF && mac[2] == 0xFF && \
     mac[3] == 0xFF  &&  mac[4] == 0xFF && mac[5] == 0xFF)

typedef enum{

  FALSE,
  TRUE 
} bool_t;

void layer2_fill_with_broadcast_mac(char *mac_array);

void apply_mask(char *prefix, char mask, char *str_prefix);





#define IS_BIT_SET(n, pos)      ((n & (1 << (pos))) != 0)
#define TOGGLE_BIT(n, pos)      (n = n ^ (1 << (pos)))
#define COMPLEMENT(num)         (num = num ^ 0xFFFFFFFF)
#define UNSET_BIT(n, pos)       (n = n & ((1 << pos) ^ 0xFFFFFFFF))
#define SET_BIT(n, pos)     (n = n | 1 << pos)

#endif*/

#ifndef __UTILS__
#define __UTILS__

#define ARP_BROAD_REQ   1
#define ARP_REPLY       2
#define ARP_MSG         806
#define BROADCAST_MAC   0xFFFFFFFFFFFF

typedef enum{

  FALSE,
  TRUE 
} bool_t;


#define IS_BIT_SET(n, pos)      ((n & (1 << (pos))) != 0)
#define TOGGLE_BIT(n, pos)      (n = n ^ (1 << (pos)))
#define COMPLEMENT(num)         (num = num ^ 0xFFFFFFFF)
#define UNSET_BIT(n, pos)       (n = n & ((1 << pos) ^ 0xFFFFFFFF))
#define SET_BIT(n, pos)     (n = n | 1 << pos)

void
apply_mask(char *prefix, char mask, char *str_prefix);

void
layer2_fill_with_broadcast_mac(char *mac_array);

#define IS_MAC_BROADCAST_ADDR(mac)   \
    (mac[0] == 0xFF  &&  mac[1] == 0xFF && mac[2] == 0xFF && \
     mac[3] == 0xFF  &&  mac[4] == 0xFF && mac[5] == 0xFF)

#endif /* __UTILS__ */
