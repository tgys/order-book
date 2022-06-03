#include <unordered_map>
#include <chrono>
#include "LOB/order_book.hh"

/*
 *     the LOB uses a priority queue to hold the bid/ask prices (so the bids are stored in a max heap
 *     with the highest bid on top and the offers in a min heap with the lowest bid on top, and
 *     because accessing the top element is O(1), finding a match (comparing 2 top elements)
 *     is also O(1)). Each queue contains a unique_ptr to a PriceOrder object which
 *     has the member variable price_t (which the comparator uses to sort the queue) and
 *     a unique_ptr to the order object (so each pointer owns an order and when it's removed
 *     from the queue the order pointed to by the unique_ptr will be destroyed.
 *     Each security has it's own bid/ask queues, stored in an unordered_map (hash table)
 *     so accessing a queue with a securityID is O(1) and inserting/deleting a PriceOrder is O(logn)
 *
 *     todo: add benchmarks and use a custom pool allocator to improve the cache locality
 *     or an alternative container (like DenseMap in the llvm library, see https://stackoverflow.com/questions/43191216/differences-similarities-between-llvmdensemap-and-stdmap)
 *     It might be possible to have constant time insertion/removals from the bid/ask queues by
 *     using a custom priority queue implemented with a linked list
 *     (see the 4th answer to https://stackoverflow.com/questions/2933758/priority-queue-with-limited-space-looking-for-a-good-algorithm)
 *     find_matches could be moved to a separate thread and the orderbooks (bid/ask queues) for each
 *     security could be locked separately so actions with a different securityID could happen concurrently
 */

namespace LOB {

    PriceOrder::PriceOrder(price_t p) : Price(p)
    {}
    void PriceOrder::set_order(std::unique_ptr<Order>& ord_ptr){
        Oid = ord_ptr->get_id();
        Ord_Ptr = std::move(ord_ptr);
    }

    void PriceOrder::invalidate(){
        Oid = -1;
    }

    order_id_t PriceOrder::oid() const{
        return Oid;
    }

    price_t PriceOrder::get_price() const { return Price; }

    Order::Order(order_id_t id, Action action, Direction direction, Constraint constraints,
                 security_id_t secID, volume_t volume, bool all_or_none, volume_t min_quantity)
        : OrderID(id), OrderAction(action), OrderDirection(direction), OrderConstraints(constraints),
        SecurityID(secID), Volume(volume), TimeCreated(std::chrono::high_resolution_clock::now())
        {}
    void Order::set_price(std::unique_ptr<PriceOrder>& price_ptr){
        Ord_Price = price_ptr.get();
    }
    void Order::dec_volume(volume_t dvol){
        Volume -= dvol;
    }
    void Order::inc_volume(volume_t dvol){
        Volume += dvol;
    }
    void Order::set_volume(volume_t vol){
        Volume = vol;
    }
    order_id_t Order::get_id() const{
        return OrderID;
    }
    security_id_t Order::get_sec_id() const{
        return SecurityID;
    }
    volume_t Order::get_volume() const{
        return Volume;
    }
    time_t Order::created_at() const{
        return TimeCreated;
    }
    Direction Order::buy_or_sell() const{
        return OrderDirection;
    }
    Agent::Agent(std::string s, std::unique_ptr<OrderBook>& order_book) : Id(s)
    {
        book = order_book.get();
    }
    OrderBook::OrderBook(){
        added_orders[-1] = std::make_pair<bool,Order*>(false,nullptr);
    }

    void OrderBook::find_matches(security_id_t secID){
        price_t spread = 0;
        while(spread <= 0){
            //find the best bid
            bool found_bid = false;
            bool found_match = false;
            while(!found_bid && !order_map[secID].first.empty()){
                //check if the top element (max bid) is valid
                if(added_orders[order_map[secID].first.top()->oid()].first){
                    //find the best offer
                    found_bid = true;
                    while(!found_match && !order_map[secID].second.empty()){
                        //check if the top element (min offer) is valid
                        if(added_orders[order_map[secID].second.top()->oid()].first){

                            spread = order_map[secID].second.top()->get_price() - order_map[secID].first.top()->get_price();
                            if(spread <= 0){
                                found_match = true;
                            } else {
                                break;
                            }
                        } else {
                            //remove the top element
                            order_map[secID].second.pop();
                        }
                    }
                } else {
                    //remove the top element
                    order_map[secID].first.pop();
                }
            }
            if(found_match){
                if(order_map[secID].first.top()->Ord_Ptr->created_at() < order_map[secID].second.top()->Ord_Ptr->created_at()){
                     //bid was created earlier
                     volume_t mvol;
                     if(order_map[secID].first.top()->Ord_Ptr->get_volume() < order_map[secID].second.top()->Ord_Ptr->get_volume()){
                         mvol = order_map[secID].first.top()->Ord_Ptr->get_volume();
                         order_map[secID].second.top()->Ord_Ptr->dec_volume(mvol);
                     } else {
                         mvol = order_map[secID].second.top()->Ord_Ptr->get_volume();
                         order_map[secID].first.top()->Ord_Ptr->dec_volume(mvol);
                     }
                     //exchange at the bid price
                } else {
                     //exchange at the offer price
                }
            }
        }
    }

    bool Agent::submit_order(order_id_t id, Action action, Direction direction, Constraint constraint,
                             price_t price, security_id_t secID, volume_t volume,
                             //only relevant for market orders
                             bool all_or_none, volume_t min_q){
        if(constraint == LIMIT){
            std::unique_ptr<Order> ord_ptr = std::make_unique<Order>(id, action, direction, constraint, secID, volume, all_or_none, min_q);
            std::unique_ptr<PriceOrder> price_ptr = std::make_unique<PriceOrder>(price);
            ord_ptr->set_price(price_ptr);
            price_ptr->set_order(ord_ptr);
            book->added_orders[id] = std::make_pair<bool,Order*>(true,ord_ptr.get());
            if(!book->order_map.contains(secID)){
                Bids b(comp_lt);
                Asks a(comp_gt);
                book->order_map[secID] = std::make_pair<Bids,Asks>(std::move(b),std::move(a));
            }
            if(direction == BUY){
                std::cout << "added a limit order to buy the security " << secID << " at the maximum price " << price << '\n';
                book->order_map[secID].first.push(std::move(price_ptr));
            } else {
                std::cout << "added a limit order to sell the security " << secID << " at the minimum price " << price << '\n';
                book->order_map[secID].second.push(std::move(price_ptr));
            }


            //find_matches(secID);
            return true;

        } else {  //MARKET order
            if(direction == BUY){
                //find the best offer
                volume_t current_volume = volume;
                bool completed = false;
                std::vector<std::unique_ptr<PriceOrder>> to_exchange;
                while(!book->order_map[secID].second.empty()){
                    //check if the top element (min offer) is valid
                    if(book->added_orders[book->order_map[secID].second.top()->oid()].first){
                        //add unique_ptrs to orders to to_exchange until the order is filled, if all_or_none is set, add
                        //the pointers back if the order cannot be completely filled.
                        //set added_orders[sell_ptr] to false (buy_ptr was never added to order_map so we don't need to invalidate it)
                        if(book->order_map[secID].second.top()->Ord_Ptr->get_volume() >= current_volume){
                            to_exchange.push_back(std::move(const_cast<std::unique_ptr<PriceOrder>&>(book->order_map[secID].second.top())));
                            book->order_map[secID].second.pop();
                            completed = true;
                            break;
                        } else {
                            current_volume -= book->order_map[secID].second.top()->oid();
                            to_exchange.push_back(std::move(const_cast<std::unique_ptr<PriceOrder>&>(book->order_map[secID].second.top())));
                            book->order_map[secID].second.pop();
                            continue;
                        }
                    } else {
                        //remove the top element
                        book->order_map[secID].second.pop();
                    }
                }
                if(!completed){
                    if(all_or_none){
                        std::cout << "could not execute the order: the total offer volume is less than the order volume";
                        //add back items in to_exchange
                        for(std::unique_ptr<PriceOrder>& p_ptr : to_exchange){
                            book->order_map[secID].second.push(std::move(p_ptr));
                        }
                        return false;
                    } else {
                        //execute partially filled order
                        return true;
                    }
                } else {
                    //execute order
                    return true;
                }
            } else {
                //find the best bid
                volume_t current_volume = volume;
                bool completed = false;
                std::vector<std::unique_ptr<PriceOrder>> to_exchange;
                while(!book->order_map[secID].first.empty()){
                    //check if the top element (max bid) is valid
                    if(book->added_orders[book->order_map[secID].first.top()->oid()].first){
                        if(book->order_map[secID].first.top()->Ord_Ptr->get_volume() >= current_volume){
                            to_exchange.push_back(std::move(const_cast<std::unique_ptr<PriceOrder>&>(book->order_map[secID].first.top())));
                            book->order_map[secID].first.pop();
                            completed = true;
                            break;
                        } else {
                            current_volume -= book->order_map[secID].first.top()->oid();
                            to_exchange.push_back(std::move(const_cast<std::unique_ptr<PriceOrder>&>(book->order_map[secID].first.top())));
                            book->order_map[secID].first.pop();
                            continue;
                        }
                    } else {
                        //remove the top element
                        book->order_map[secID].first.pop();
                    }
                }
                if(!completed){
                    if(all_or_none){
                        std::cout << "could not execute the order: the total offer volume is less than the order volume";
                        //add back items in to_exchange
                        for(std::unique_ptr<PriceOrder>& p_ptr : to_exchange){
                            book->order_map[secID].first.push(std::move(p_ptr));
                        }
                        return false;
                    } else {
                        //execute partially filled order
                        return true;
                    }
                } else {
                    //execute order
                    return true;
                }

            }
        }
    }

    bool Agent::cancel_order(order_id_t id){
        if(book->added_orders.contains(id)){
            book->added_orders[id].first = false;
            return true;
        } else {
            return false;
        }
    }

    bool Agent::modify_order_direction(order_id_t id, Direction direction){

        if(book->added_orders.contains(id)){
            //added_orders[id].first = false;
            Order* ord = book->added_orders[id].second;
            ord->Ord_Price->invalidate();
            std::unique_ptr<PriceOrder> price_ptr = std::make_unique<PriceOrder>(ord->Ord_Price->get_price());
            //transfer ownership of the unique_ptr (owned by the invalidated PriceOrder) to the new PriceOrder
            price_ptr->set_order(ord->Ord_Price->Ord_Ptr);
            ord->set_price(price_ptr);
            if(direction == BUY){
                std::cout << "modified the order direction of " << id << ", sell->buy" << '\n';
                book->order_map[ord->get_sec_id()].first.push(std::move(price_ptr));
            } else {
                std::cout << "modified the order direction of " << id << ", buy->sell" << '\n';
                book->order_map[ord->get_sec_id()].second.push(std::move(price_ptr));
            }
            return true;
        } else {
            return false;
        }
        //find_matches(secID)
    }
    bool Agent::modify_order_volume(order_id_t id, volume_t volume){

        if(book->added_orders.contains(id)){
            //added_orders[id].first = false;
            Order* ord = book->added_orders[id].second;
            ord->set_volume(volume);
            return true;
        } else {
            return false;
        }
        //find_matches(secID)
    }
    bool Agent::modify_order_price(order_id_t id, price_t price){

        if(book->added_orders.contains(id)){
            //added_orders[id].first = false;
            Order* ord = book->added_orders[id].second;
            //set the order id to -1 (which is always invalid) and insert a new PriceOrder object
            //with the new price
            ord->Ord_Price->invalidate();
            std::unique_ptr<PriceOrder> price_ptr = std::make_unique<PriceOrder>(price);
            //transfer ownership of the unique_ptr (owned by the invalidated PriceOrder) to the new PriceOrder
            price_ptr->set_order(ord->Ord_Price->Ord_Ptr);
            ord->set_price(price_ptr);
            if(ord->buy_or_sell() == BUY){
                std::cout << "modified the buy order " << id << ", set the max price to " << price << '\n';
                book->order_map[ord->get_sec_id()].first.push(std::move(price_ptr));
            } else {
                std::cout << "modified the sell order " << id << ", set the min price to " << price << '\n';
                book->order_map[ord->get_sec_id()].second.push(std::move(price_ptr));
            }
            return true;
        } else {
            return false;
        }
        //find_matches(secID)
        //whenever an order is inserted into the bid/ask queue (or modified), look for
        //a matching order in the other queue
    }

    void OrderBook::execute(Agent& a1, std::unordered_map<order_id_t, Agent>& agents, std::vector<PriceOrder*>& to_exchange){

    }

    void OrderBook::exchange(Agent& a1, Agent& a2, Direction d, volume_t n, security_id_t sid){
        if(d == BUY){  //assume seller always has at least n
            a1.increase_asset(sid,n);
            a2.decrease_asset(sid,n);
        } else {
            a1.decrease_asset(sid,n);
            a2.increase_asset(sid,n);
        }
        std::cout << "agent " << a1.Id << " " << (d == BUY ? "brought" : "sold") << " "
        << n << " units of security " << sid << (d == BUY ? "from" : "to") << " agent " << a2.Id << '\n';
    }

} /* end namespace LOB */
