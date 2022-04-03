#include "ccex_order_executor.hpp"
#include "program_options.hpp"

#include <ccapi_cpp/ccapi_macro.h>

#include <boost/log/trivial.hpp>
#include <boost/json.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <map>

namespace profitview
{

struct ProgramArgs
{
	std::string exchange;
    std::string symbol;
	std::string apiKey;
	std::string apiSecret;
	std::string apiPhrase;
    Side side = Side::Buy;
	double size = 0.0;
    OrderType type = OrderType::Limit;
	double price = 0.0;

	void addOptions(boost::program_options::options_description& options)
	{
        namespace po = boost::program_options;
		options.add_options()
			("exchange", po::value(&exchange)->required(), "Crypto Exchange to execute on.")
			("symbol", po::value(&symbol)->required(), "Symbol for cypto assets to trade.")
			("api_key", po::value(&apiKey)->required(), "API key for Cypto exchange.")
			("api_secret", po::value(&apiSecret)->required(), "API secret for Cypto exchange.")
			("api_phrase", po::value(&apiPhrase), "API phrase for Cypto exchange.")
			("side", po::value(&side)->required(), "The side of the trade <buy|sell>.")
			("size", po::value(&size)->required(), "Size to trade.")
			("type", po::value(&type)->required(), "The type of order <limit|market>.")
    		("price", po::value(&price), "Price to trade at.")
		;		
	}
};

}
auto main(int argc, char* argv[]) -> int
{
	using namespace profitview;
	ProgramArgs options;
	auto const result = parseProgramOptions(argc, argv, options);
	if (result)
		return result.value();

    const std::map<std::string, std::string> exchange_names = {
        {"ftx", CCAPI_EXCHANGE_NAME_FTX},
        {"coinbase", CCAPI_EXCHANGE_NAME_COINBASE},
        {"bitmex", CCAPI_EXCHANGE_NAME_BITMEX},
    };

    BOOST_LOG_TRIVIAL(info) << "Running Ccex test.";

    CcexOrderExecutor executor{
        exchange_names.at(options.exchange), 
        options.apiKey, 
        options.apiSecret, 
        options.apiPhrase
    };

    BOOST_LOG_TRIVIAL(info) << options.symbol << "Running: " << std::endl; 
    executor.new_order(options.symbol, options.side, options.size, options.type, options.price);

	enum {OrderId, Symbol, OrderSide, Size, Price, Time, Status};
	for(const auto& [cid, details]: executor.get_open_orders())
		std::cout 
			<< "cid: " << cid 
			<< ", symbol: " << std::get<OrderId>(details) 
			<< ", status: " << std::get<Status>(details) 
			<< std::endl; 
    return 0;
}