//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2022
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/telegram/DialogId.h"
#include "td/telegram/files/FileId.h"
#include "td/telegram/FullMessageId.h"
#include "td/telegram/MessageId.h"
#include "td/telegram/Photo.h"
#include "td/telegram/td_api.h"
#include "td/telegram/telegram_api.h"

#include "td/utils/common.h"
#include "td/utils/Promise.h"
#include "td/utils/Slice.h"
#include "td/utils/Status.h"
#include "td/utils/StringBuilder.h"

namespace td {

class Td;

struct LabeledPricePart {
  string label;
  int64 amount = 0;

  LabeledPricePart() = default;
  LabeledPricePart(string &&label, int64 amount) : label(std::move(label)), amount(amount) {
  }
};

struct Invoice {
  string currency;
  vector<LabeledPricePart> price_parts;
  int64 max_tip_amount = 0;
  vector<int64> suggested_tip_amounts;
  string recurring_payment_terms_of_service_url;
  bool is_test = false;
  bool need_name = false;
  bool need_phone_number = false;
  bool need_email_address = false;
  bool need_shipping_address = false;
  bool send_phone_number_to_provider = false;
  bool send_email_address_to_provider = false;
  bool is_flexible = false;

  Invoice() = default;
  Invoice(string &&currency, bool is_test, bool need_shipping_address)
      : currency(std::move(currency)), is_test(is_test), need_shipping_address(need_shipping_address) {
  }
};

struct InputInvoice {
  string title;
  string description;
  Photo photo;
  string start_parameter;
  Invoice invoice;
  string payload;
  string provider_token;
  string provider_data;

  int64 total_amount = 0;
  MessageId receipt_message_id;
};

struct Address {
  string country_code;
  string state;
  string city;
  string street_line1;
  string street_line2;
  string postal_code;

  Address() = default;
  Address(string &&country_code, string &&state, string &&city, string &&street_line1, string &&street_line2,
          string &&postal_code)
      : country_code(std::move(country_code))
      , state(std::move(state))
      , city(std::move(city))
      , street_line1(std::move(street_line1))
      , street_line2(std::move(street_line2))
      , postal_code(std::move(postal_code)) {
  }
};

struct OrderInfo {
  string name;
  string phone_number;
  string email_address;
  unique_ptr<Address> shipping_address;

  OrderInfo() = default;
  OrderInfo(string &&name, string &&phone_number, string &&email_address, unique_ptr<Address> &&shipping_address)
      : name(std::move(name))
      , phone_number(std::move(phone_number))
      , email_address(std::move(email_address))
      , shipping_address(std::move(shipping_address)) {
  }
};

struct ShippingOption {
  string id;
  string title;
  vector<LabeledPricePart> price_parts;
};

bool operator==(const LabeledPricePart &lhs, const LabeledPricePart &rhs);
bool operator!=(const LabeledPricePart &lhs, const LabeledPricePart &rhs);

StringBuilder &operator<<(StringBuilder &string_builder, const LabeledPricePart &labeled_price_part);

bool operator==(const Invoice &lhs, const Invoice &rhs);
bool operator!=(const Invoice &lhs, const Invoice &rhs);

StringBuilder &operator<<(StringBuilder &string_builder, const Invoice &invoice);

bool operator==(const InputInvoice &lhs, const InputInvoice &rhs);
bool operator!=(const InputInvoice &lhs, const InputInvoice &rhs);

InputInvoice get_input_invoice(tl_object_ptr<telegram_api::messageMediaInvoice> &&message_invoice, Td *td,
                               DialogId owner_dialog_id);

InputInvoice get_input_invoice(tl_object_ptr<telegram_api::botInlineMessageMediaInvoice> &&message_invoice, Td *td,
                               DialogId owner_dialog_id);

Result<InputInvoice> process_input_message_invoice(
    td_api::object_ptr<td_api::InputMessageContent> &&input_message_content, Td *td);

tl_object_ptr<td_api::messageInvoice> get_message_invoice_object(const InputInvoice &input_invoice, Td *td);

tl_object_ptr<telegram_api::inputMediaInvoice> get_input_media_invoice(const InputInvoice &input_invoice, Td *td);

tl_object_ptr<telegram_api::inputBotInlineMessageMediaInvoice> get_input_bot_inline_message_media_invoice(
    const InputInvoice &input_invoice, tl_object_ptr<telegram_api::ReplyMarkup> &&reply_markup, Td *td);

vector<FileId> get_input_invoice_file_ids(const InputInvoice &input_invoice);

bool operator==(const Address &lhs, const Address &rhs);
bool operator!=(const Address &lhs, const Address &rhs);

StringBuilder &operator<<(StringBuilder &string_builder, const Address &address);

unique_ptr<Address> get_address(tl_object_ptr<telegram_api::postAddress> &&address);

Result<Address> get_address(td_api::object_ptr<td_api::address> &&address);

tl_object_ptr<td_api::address> get_address_object(const unique_ptr<Address> &address);

tl_object_ptr<td_api::address> get_address_object(const Address &address);

string address_to_json(const Address &address);

Result<Address> address_from_json(Slice json);

Status check_country_code(string &country_code);

bool operator==(const OrderInfo &lhs, const OrderInfo &rhs);
bool operator!=(const OrderInfo &lhs, const OrderInfo &rhs);

StringBuilder &operator<<(StringBuilder &string_builder, const OrderInfo &order_info);

unique_ptr<OrderInfo> get_order_info(tl_object_ptr<telegram_api::paymentRequestedInfo> order_info);

tl_object_ptr<td_api::orderInfo> get_order_info_object(const unique_ptr<OrderInfo> &order_info);

bool operator==(const ShippingOption &lhs, const ShippingOption &rhs);
bool operator!=(const ShippingOption &lhs, const ShippingOption &rhs);

StringBuilder &operator<<(StringBuilder &string_builder, const ShippingOption &shipping_option);

void answer_shipping_query(Td *td, int64 shipping_query_id,
                           vector<tl_object_ptr<td_api::shippingOption>> &&shipping_options,
                           const string &error_message, Promise<Unit> &&promise);

void answer_pre_checkout_query(Td *td, int64 pre_checkout_query_id, const string &error_message,
                               Promise<Unit> &&promise);

void get_payment_form(Td *td, td_api::object_ptr<td_api::InputInvoice> &&input_invoice,
                      const td_api::object_ptr<td_api::themeParameters> &theme,
                      Promise<tl_object_ptr<td_api::paymentForm>> &&promise);

void validate_order_info(Td *td, td_api::object_ptr<td_api::InputInvoice> &&input_invoice,
                         td_api::object_ptr<td_api::orderInfo> &&order_info, bool allow_save,
                         Promise<td_api::object_ptr<td_api::validatedOrderInfo>> &&promise);

void send_payment_form(Td *td, td_api::object_ptr<td_api::InputInvoice> &&input_invoice, int64 payment_form_id,
                       const string &order_info_id, const string &shipping_option_id,
                       const td_api::object_ptr<td_api::InputCredentials> &credentials, int64 tip_amount,
                       Promise<td_api::object_ptr<td_api::paymentResult>> &&promise);

void get_payment_receipt(Td *td, FullMessageId full_message_id,
                         Promise<tl_object_ptr<td_api::paymentReceipt>> &&promise);

void get_saved_order_info(Td *td, Promise<tl_object_ptr<td_api::orderInfo>> &&promise);

void delete_saved_order_info(Td *td, Promise<Unit> &&promise);

void delete_saved_credentials(Td *td, Promise<Unit> &&promise);

void export_invoice(Td *td, td_api::object_ptr<td_api::InputMessageContent> &&invoice, Promise<string> &&promise);

void get_bank_card_info(Td *td, const string &bank_card_number,
                        Promise<td_api::object_ptr<td_api::bankCardInfo>> &&promise);

}  // namespace td
