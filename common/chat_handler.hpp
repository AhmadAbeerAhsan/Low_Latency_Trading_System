#include <memory>
#include <boost/asio.hpp>
#include <deque>
class chat_handler : public std::enable_shared_from_this<chat_handler>
{
public:
    chat_handler(boost::asio::io_context &io_context)
        : io_context_(io_context),
          socket_(io_context),
          write_strand_(boost::asio::make_strand(io_context)) {};

    boost::asio::ip::tcp::socket &socket()
    {
        return socket_;
    }

    void start()
    {
        read_packet();
    }

    void send(std::string msg)
    {
        auto handler = [me = shared_from_this()](boost::system::error_code const &ec, std::size_t)
        {
            me->packet_send_done(ec);
        };

        // Associate the handler with strand (serialized execution)
        auto bound_handler = boost::asio::bind_executor(write_strand_, handler);

        // Get the allocator associated with the handler
        auto allocator = boost::asio::get_associated_allocator(bound_handler);

        // Post with executor and allocator
        io_context_.get_executor().post(bound_handler, allocator);
    }

private:
    void read_packet()
    {
        boost::asio::async_read_until(
            socket_, in_packet_, '\0',
            [me = shared_from_this()](boost::system::error_code const &ec, std::size_t bytes_xfer)
            {
                me->read_packet_done(ec, bytes_xfer);
            });
    }

    void read_packet_done(boost::system::error_code const &error, std::size_t bytes_transferred)
    {
        if (error)
        {
            return;
        }
        std::istream stream(&in_packet_);
        std::string packet_string;
        stream >> packet_string;
        // do something with it
        read_packet();
    }

    void queue_message(std::string message)
    {
        bool write_in_progress = !send_packet_queue.empty();
        send_packet_queue.push_back(std::move(message));
        if (!write_in_progress)
        {
            start_packet_send();
        }
    }

    void packet_send_done(boost::system::error_code const &error)
    {
        if (!error)
        {
            send_packet_queue.pop_front();
            if (!send_packet_queue.empty())
            {
                start_packet_send();
            }
        }
    }

    boost::asio::io_context &io_context_;
    boost::asio::ip::tcp::socket socket_;                                      // socket used to communicate with client
    boost::asio::strand<boost::asio::io_context::executor_type> write_strand_; // to prevent multiple writes on sockets
    boost::asio::streambuf in_packet_;                                         // for receiving data in
    std::deque<std::string> send_packet_queue;                                 // for sending data in
};