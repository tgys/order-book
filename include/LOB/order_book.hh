#ifndef LOB_FOO_H_
#define LOB_FOO_H_

#include <iostream>
#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <chrono>

//todo: add const

namespace LOB {

    using security_id_t = std::int32_t;
    using order_id_t = long long;
    using volume_t = unsigned long long;
    using price_t = long double;
    using time_t = std::chrono::time_point<std::chrono::system_clock>;

    enum Action{
        Add = 0,
        Modify = 1,
        Delete = 2
    };

    enum Direction{
        BUY = 0,
        SELL = 1,
    };

    enum Constraint{
        LIMIT = 0,
        MARKET = 1,
        //   STOP_LIMIT = 2,
        //   STOP_MARKET = 3,
        //   ICEBERG = 4
    };

    class PriceOrder;
    class Order;
    class OrderBook;
    //template<class Alloc> using Asks = std::priority_queue<price_t, std::vector<price_t, Alloc>, std::greater<price_t>>;
    //template<class Alloc> using Bids = std::priority_queue<price_t, std::vector<price_t, Alloc>, std::less<price_t>>;
    // std::unordered_map<security_id_t,std::pair<Asks<pool_alloc>>> order_map_pool;

    class PriceOrder {
        price_t Price;
        order_id_t Oid;
    public:
        std::unique_ptr<Order> Ord_Ptr;
        explicit PriceOrder(price_t p);
        void set_order(std::unique_ptr<Order>& ord_ptr);
        void invalidate();
        order_id_t oid() const;
        [[nodiscard]] price_t get_price() const;
    };


    bool comp_lt(std::unique_ptr<PriceOrder>& p1, std::unique_ptr<PriceOrder>& p2){ return p1->get_price() < p2->get_price(); }
    bool comp_gt(std::unique_ptr<PriceOrder>& p1, std::unique_ptr<PriceOrder>& p2){ return p1->get_price() > p2->get_price(); }
    typedef bool (*comp)(std::unique_ptr<PriceOrder>& p1, std::unique_ptr<PriceOrder>& p2);
    using Bids = std::priority_queue<std::unique_ptr<PriceOrder>, std::vector<std::unique_ptr<PriceOrder>>, comp>;
    using Asks = std::priority_queue<std::unique_ptr<PriceOrder>, std::vector<std::unique_ptr<PriceOrder>>, comp>;

    class Agent {
    public:
        std::string Id;
        std::map<security_id_t, volume_t> Assets;
        OrderBook* book;
        volume_t get_quantity(security_id_t sid){
            return Assets[sid];
        }
        void increase_asset(security_id_t sid, volume_t quantity){
            if(Assets.contains(sid)){
                Assets[sid] += quantity;
            } else {
                Assets[sid] = quantity;
            }
        }
        void decrease_asset(security_id_t sid, volume_t quantity){
            if(Assets.contains(sid)){
                Assets[sid] -= quantity;
            }
        }
        //returns false if the order did not complete
        bool submit_order(order_id_t id, Action action, Direction direction, Constraint constraint,
                          price_t price, security_id_t secID, volume_t volume, bool all_or_none = false, volume_t min_q = 1);

        //returns false if the order doesn't exist
        bool modify_order_direction(order_id_t id, Direction direction);
        bool modify_order_price(order_id_t id, price_t price);
        bool modify_order_volume(order_id_t id, volume_t volume);
        bool cancel_order(order_id_t id);
        explicit Agent(std::string s, std::unique_ptr<OrderBook>& book);
    };

    class OrderBook {
        friend class Agent;
    protected:
        std::unordered_map<security_id_t,std::pair<Bids, Asks>> order_map;
        std::unordered_map<order_id_t,std::pair<bool,Order*>> added_orders;
        std::unordered_map<order_id_t, Agent> agent_orders;
        std::unordered_map<std::string,Agent> agents;
    public:
        void find_matches(security_id_t secID);
        void execute(Agent& a1, std::unordered_map<order_id_t, Agent>& agents, std::vector<PriceOrder*>& to_exchange);
        void exchange(Agent& a1, Agent& a2, Direction d, volume_t n, security_id_t sid);
        explicit OrderBook();
    };

    class Order {
        order_id_t OrderID;
        Action OrderAction;
        Direction OrderDirection;
        Constraint OrderConstraints;
        security_id_t SecurityID;
        volume_t Volume;
        time_t TimeCreated;

    public:
        PriceOrder* Ord_Price;
        Order(order_id_t id, Action action, Direction direction,
              Constraint constraint, security_id_t secID, volume_t volume, bool all_or_none, volume_t min_quantity);
        void set_price(std::unique_ptr<PriceOrder>& price_ptr);
        order_id_t get_id() const;
        volume_t get_volume() const;
        Direction buy_or_sell() const;
        security_id_t get_sec_id() const;
        void dec_volume(volume_t dvol);
        void inc_volume(volume_t dvol);
        void set_volume(volume_t vol);
        time_t created_at() const;
    };

}

#endif
